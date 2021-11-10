#include "Asset/ModelLoader.hpp"
#include "Asset/Image.hpp"
#include "Renderer/Material.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace sd {

static Vertex constructVertex(const aiVector3D &pos, const aiVector3D &texCoord,
                              const aiVector3D &normal,
                              const aiVector3D &tangent,
                              const aiVector3D &biTangent) {
    Vertex vertex;
    vertex.position.x = pos.x;
    vertex.position.y = pos.y;
    vertex.position.z = pos.z;
    vertex.texCoord.x = texCoord.x;
    vertex.texCoord.y = texCoord.y;
    vertex.normal.x = normal.x;
    vertex.normal.y = normal.y;
    vertex.normal.z = normal.z;
    vertex.tangent.x = tangent.x;
    vertex.tangent.y = tangent.y;
    vertex.tangent.z = tangent.z;
    vertex.biTangent.x = biTangent.x;
    vertex.biTangent.y = biTangent.y;
    vertex.biTangent.z = biTangent.z;
    return vertex;
}

static MeshTopology convertAssimpPrimitive(aiPrimitiveType type) {
    switch (type) {
        case aiPrimitiveType_LINE:
            return MeshTopology::LINES;
        case aiPrimitiveType_POINT:
            return MeshTopology::POINTS;
        case aiPrimitiveType_TRIANGLE:
            return MeshTopology::TRIANGLES;
        case aiPrimitiveType_POLYGON:
            return MeshTopology::QUADS;
        default:
            SD_CORE_WARN("[convertAssimpPrimitive] Unhandled mesh topology!");
            return MeshTopology::TRIANGLES;
    };
}

static Mesh processAiMesh(const aiMesh *assimpMesh) {
    const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < assimpMesh->mNumVertices; ++i) {
        const aiVector3D pos = assimpMesh->mVertices[i];
        const aiVector3D normal =
            assimpMesh->HasNormals() ? assimpMesh->mNormals[i] : aiZeroVector;
        const aiVector3D texCoord = assimpMesh->HasTextureCoords(0)
                                        ? assimpMesh->mTextureCoords[0][i]
                                        : aiZeroVector;
        const aiVector3D tangent = assimpMesh->HasTangentsAndBitangents()
                                       ? assimpMesh->mTangents[i]
                                       : aiZeroVector;
        const aiVector3D biTangent = assimpMesh->HasTangentsAndBitangents()
                                         ? assimpMesh->mBitangents[i]
                                         : aiZeroVector;
        vertices.push_back(
            constructVertex(pos, texCoord, normal, tangent, biTangent));
    }
    for (uint32_t i = 0; i < assimpMesh->mNumFaces; ++i) {
        const aiFace &face = assimpMesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    Mesh mesh(vertices, indices);
    mesh.setTopology(convertAssimpPrimitive(
        static_cast<aiPrimitiveType>(assimpMesh->mPrimitiveTypes)));
    mesh.setMaterialIndex(assimpMesh->mMaterialIndex);
    return mesh;
}

static TextureWrap convertAssimpMapMode(aiTextureMapMode mode) {
    switch (mode) {
        case aiTextureMapMode_Clamp:
            return TextureWrap::EDGE;
        case aiTextureMapMode_Decal:
            return TextureWrap::BORDER;
        case aiTextureMapMode_Mirror:
            return TextureWrap::MIRROR;
        case aiTextureMapMode_Wrap:
            return TextureWrap::REPEAT;
        default:
            SD_CORE_WARN("[convertAssimpMapMode] invalid wrap mode!");
            return TextureWrap::REPEAT;
    }
}

static void processAiMaterial(const std::filesystem::path &directory,
                              Material &material, MaterialType materialType,
                              const aiMaterial *assimpMaterial,
                              aiTextureType assimpType) {
    uint32_t count = assimpMaterial->GetTextureCount(assimpType);
    if (count < 1) {
        return;
    } else if (count > 1) {
        SD_CORE_WARN(
            "[processAiMaterial] Cannot handle multiple texture of same type!");
    }

    aiString texturePath;
    aiTextureMapMode wrapMode;
    if (assimpMaterial->GetTexture(assimpType, 0, &texturePath, nullptr,
                                   nullptr, nullptr, nullptr,
                                   &wrapMode) != AI_SUCCESS) {
        SD_CORE_ERROR("[processAiMaterial] Assimp GetTexture error!");
        return;
    }
    std::string path = (directory / texturePath.C_Str()).string();
    Asset asset = AssetManager::instance().loadAsset<Image>(path);
    auto image = asset.getResource<Image>();
    auto texture = Texture::create(
        image->width(), image->height(), 1, TextureType::TEX_2D,
        image->hasAlpha() ? TextureFormat::RGBA : TextureFormat::RGB,
        TextureFormatType::UBYTE, convertAssimpMapMode(wrapMode),
        TextureFilter::LINEAR, TextureMipmapFilter::LINEAR_LINEAR,
        image->data());
    material.setTexture(materialType, texture);
}

static void processNode(const aiScene *scene, const aiNode *node,
                        Ref<Model> &model) {
    if (node == nullptr) return;
    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        const aiNode *child = node->mChildren[i];
        for (uint32_t j = 0; j < child->mNumMeshes; ++j) {
            uint32_t id = child->mMeshes[j];
            model->addMesh(processAiMesh(scene->mMeshes[id]));
        }
        processNode(scene, child, model);
    }
}

Ref<void> ModelLoader::loadAsset(const std::string &filePath) {
    Ref<Model> model = createRef<Model>();
    SD_CORE_TRACE("Loading model form: {}...", filePath);

    Assimp::Importer importer;
    uint32_t importFlags = aiProcess_Triangulate | aiProcess_FlipUVs;
    const aiScene *scene = importer.ReadFile(filePath, importFlags);
    if (scene == nullptr) {
        std::string error = importer.GetErrorString();
        SD_CORE_ERROR("Model loading failed: {}", error.c_str());
        return model;
    }

    processNode(scene, scene->mRootNode, model);
    std::filesystem::path directory =
        std::filesystem::path(filePath).parent_path();
    for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
        Material material;
        processAiMaterial(directory, material, MaterialType::DIFFUSE,
                          scene->mMaterials[i], aiTextureType_DIFFUSE);
        processAiMaterial(directory, material, MaterialType::SPECULAR,
                          scene->mMaterials[i], aiTextureType_SPECULAR);
        processAiMaterial(directory, material, MaterialType::AMBIENT,
                          scene->mMaterials[i], aiTextureType_AMBIENT);
        processAiMaterial(directory, material, MaterialType::EMISSIVE,
                          scene->mMaterials[i], aiTextureType_EMISSIVE);
        processAiMaterial(directory, material, MaterialType::HEIGHT,
                          scene->mMaterials[i], aiTextureType_HEIGHT);
        processAiMaterial(directory, material, MaterialType::NORMALS,
                          scene->mMaterials[i], aiTextureType_NORMALS);
        processAiMaterial(directory, material, MaterialType::SHININESS,
                          scene->mMaterials[i], aiTextureType_SHININESS);
        model->addMaterial(std::move(material));
    }

    return model;
}

}  // namespace sd
