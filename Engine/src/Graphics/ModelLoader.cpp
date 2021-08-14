#include "Graphics/ModelLoader.hpp"
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

static Mesh processAiMesh(const aiMesh *mesh) {
    Mesh model;
    const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D pos = mesh->mVertices[i];
        const aiVector3D normal =
            mesh->HasNormals() ? mesh->mNormals[i] : aiZeroVector;
        const aiVector3D texCoord = mesh->HasTextureCoords(0)
                                        ? mesh->mTextureCoords[0][i]
                                        : aiZeroVector;
        const aiVector3D tangent = mesh->HasTangentsAndBitangents()
                                       ? mesh->mTangents[i]
                                       : aiZeroVector;
        const aiVector3D biTangent = mesh->HasTangentsAndBitangents()
                                         ? mesh->mBitangents[i]
                                         : aiZeroVector;
        model.addVertex(
            constructVertex(pos, texCoord, normal, tangent, biTangent));
    }
    for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace &face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j) {
            model.addIndex(face.mIndices[j]);
        }
    }
    return model;
}

Ref<Model> ModelLoader::loadAsset(const std::string &filePath) {
    Ref<Model> model = createRef<Model>();
    std::string fullPath = m_manager.getRootPath() + filePath;
    SD_CORE_TRACE("Loading mesh form: {}...", fullPath);

    Assimp::Importer importer;
    uint32_t importFlags = aiProcess_Triangulate | aiProcess_FlipUVs;
    const aiScene *scene = importer.ReadFile(fullPath, importFlags);
    if (scene == nullptr) {
        std::string error = importer.GetErrorString();
        SD_CORE_ERROR("Mesh loading failed: {}", error.c_str());
        return model;
    }

    for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        model->addMesh(processAiMesh(scene->mMeshes[i]));
    }

    model->init();
    return model;
}

}  // namespace sd
