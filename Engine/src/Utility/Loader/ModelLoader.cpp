#include "Utility/Loader/ModelLoader.hpp"
#include "Graphics/Material.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace sd {

ModelLoader::ModelLoader(AssetManager &manager) : AssetLoader<Model>(manager) {}

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

static Mesh processAiMesh(const aiMesh *assimpMesh) {
    Mesh mesh;
    const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
    MeshTopology topology;
    switch (assimpMesh->mPrimitiveTypes) {
        case aiPrimitiveType_LINE:
            topology = MeshTopology::LINES;
            break;
        case aiPrimitiveType_POINT:
            topology = MeshTopology::POINTS;
            break;
        case aiPrimitiveType_TRIANGLE:
            topology = MeshTopology::TRIANGLES;
            break;
        case aiPrimitiveType_POLYGON:
            topology = MeshTopology::QUADS;
            break;
        default:
            topology = MeshTopology::TRIANGLES;
            SD_CORE_WARN("[processAiMesh] Unhandled mesh topology!");
    };
    mesh.setTopology(topology);
    mesh.setMaterialIndex(assimpMesh->mMaterialIndex);

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
        mesh.addVertex(
            constructVertex(pos, texCoord, normal, tangent, biTangent));
    }
    for (uint32_t i = 0; i < assimpMesh->mNumFaces; ++i) {
        const aiFace &face = assimpMesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j) {
            mesh.addIndex(face.mIndices[j]);
        }
    }
    return mesh;
}

static void processAiMaterial(const std::filesystem::path &directory,
                              Material &material, MaterialType materialType,
                              const aiMaterial *assimpMaterial,
                              aiTextureType assimpType) {
    uint32_t count = assimpMaterial->GetTextureCount(assimpType);
    if (material.hasTexture(materialType) || count < 1) return;

    if (count > 1)
        SD_CORE_WARN(
            "[processAiMaterial] Cannot handle multiple texture of same type!");

    aiString texturePath;
    for (uint32_t i = 0; i < count; ++i) {
        if (assimpMaterial->GetTexture(assimpType, i, &texturePath) !=
            AI_SUCCESS) {
            SD_CORE_ERROR("[processAiMaterial] Assimp GetTexture error!");
            continue;
        }
        std::string path = (directory / texturePath.C_Str()).string();
        auto texture = Asset::manager().load<Texture>(path);
        texture->setMipmapFilter(TextureMipmapFilter::LINEAR_LINEAR);
        material.setTexture(materialType, texture);
    }
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

Ref<Model> ModelLoader::loadAsset(const std::string &filePath) {
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
        model->addMaterial(std::move(material));
    }

    model->init();
    return model;
}

}  // namespace sd
