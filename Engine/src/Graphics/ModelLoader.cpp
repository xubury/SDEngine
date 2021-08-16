#include "Graphics/ModelLoader.hpp"
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

static Ref<Texture> processAiMaterial(const std::filesystem::path &directory,
                                      const aiMaterial *assimpMaterial,
                                      aiTextureType type) {
    uint32_t count = assimpMaterial->GetTextureCount(type);
    if (count > 1) SD_CORE_WARN("Cannot handle multiple texture of same type!");
    aiString texturePath;
    Ref<Texture> texture;
    for (uint32_t i = 0; i < count; ++i) {
        if (assimpMaterial->GetTexture(type, i, &texturePath) == AI_SUCCESS) {
            texture = Graphics::assetManager().load<sd::Texture>(
                directory / texturePath.C_Str());
        } else {
            SD_CORE_ERROR("Assimp GetTexture error!");
        }
    }
    return texture;
}

Ref<Model> ModelLoader::loadAsset(const std::string &filePath) {
    Ref<Model> model = createRef<Model>();
    SD_CORE_TRACE("Loading mesh form: {}...", filePath);

    Assimp::Importer importer;
    uint32_t importFlags = aiProcess_Triangulate | aiProcess_FlipUVs;
    const aiScene *scene = importer.ReadFile(filePath, importFlags);
    if (scene == nullptr) {
        std::string error = importer.GetErrorString();
        SD_CORE_ERROR("Mesh loading failed: {}", error.c_str());
        return model;
    }

    for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        model->addMesh(processAiMesh(scene->mMeshes[i]));
    }

    std::filesystem::path directory =
        std::filesystem::path(filePath).parent_path();
    for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
        Material material;

        material.addTexture(MaterialType::DIFFUSE,
                            processAiMaterial(directory, scene->mMaterials[i],
                                              aiTextureType_DIFFUSE));
        material.addTexture(MaterialType::SPECULAR,
                            processAiMaterial(directory, scene->mMaterials[i],
                                              aiTextureType_SPECULAR));
        material.addTexture(MaterialType::AMBIENT,
                            processAiMaterial(directory, scene->mMaterials[i],
                                              aiTextureType_AMBIENT));

        model->addMaterial(std::move(material));
    }

    model->init();
    return model;
}

}  // namespace sd
