#include "Graphics/MaterialLoader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace sd {

static void processAiMaterial(const std::filesystem::path &directory,
                              Material &material, MaterialType materialType,
                              const aiMaterial *assimpMaterial,
                              aiTextureType assimpType) {
    uint32_t count = assimpMaterial->GetTextureCount(assimpType);
    if (material.hasTexture(materialType) || count < 1) return;

    if (count > 1) SD_CORE_WARN("Cannot handle multiple texture of same type!");

    Material::TextureProp prop;
    aiString texturePath;
    for (uint32_t i = 0; i < count; ++i) {
        if (assimpMaterial->GetTexture(assimpType, i, &texturePath) ==
            AI_SUCCESS) {
            prop.isColor = false;
            prop.path = (directory / texturePath.C_Str()).string();
        } else {
            SD_CORE_ERROR("Assimp GetTexture error!");
        }
    }
    material.setTexture(materialType, prop);
}

MaterialLoader::MaterialLoader(AssetManager &manager)
    : AssetLoader<Material>(manager) {}

Ref<Material> MaterialLoader::loadAsset(const std::string &filePath) {
    Ref<Material> material = createRef<Material>();
    SD_CORE_TRACE("Loading material form: {}...", filePath);

    Assimp::Importer importer;
    uint32_t importFlags = aiProcess_Triangulate | aiProcess_FlipUVs;
    const aiScene *scene = importer.ReadFile(filePath, importFlags);
    if (scene == nullptr) {
        std::string error = importer.GetErrorString();
        SD_CORE_ERROR("Mesh loading failed: {}", error.c_str());
        return material;
    }

    std::filesystem::path directory =
        std::filesystem::path(filePath).parent_path();
    for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
        processAiMaterial(directory, *material, MaterialType::DIFFUSE,
                          scene->mMaterials[i], aiTextureType_DIFFUSE);
        processAiMaterial(directory, *material, MaterialType::SPECULAR,
                          scene->mMaterials[i], aiTextureType_SPECULAR);
        processAiMaterial(directory, *material, MaterialType::AMBIENT,
                          scene->mMaterials[i], aiTextureType_AMBIENT);
    }

    material->init();
    return material;
}

}  // namespace sd
