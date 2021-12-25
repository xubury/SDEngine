#include "Renderer/ModelLoader.hpp"
#include "Renderer/Model.hpp"
#include "Renderer/Bitmap.hpp"

#include "Utility/ThreadPool.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace SD {

static Vertex ConstructVertex(const aiVector3D &pos, const aiVector3D &uv,
                              const aiVector3D &normal,
                              const aiVector3D &tangent,
                              const aiVector3D &bi_tangent) {
    Vertex vertex;
    vertex.position.x = pos.x;
    vertex.position.y = pos.y;
    vertex.position.z = pos.z;
    vertex.uv.x = uv.x;
    vertex.uv.y = uv.y;
    vertex.normal.x = normal.x;
    vertex.normal.y = normal.y;
    vertex.normal.z = normal.z;
    vertex.tangent.x = tangent.x;
    vertex.tangent.y = tangent.y;
    vertex.tangent.z = tangent.z;
    vertex.bi_tangent.x = bi_tangent.x;
    vertex.bi_tangent.y = bi_tangent.y;
    vertex.bi_tangent.z = bi_tangent.z;
    return vertex;
}

static MeshTopology ConvertAssimpPrimitive(aiPrimitiveType type) {
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
            SD_CORE_WARN("[ConvertAssimpPrimitive] Unhandled mesh topology!");
            return MeshTopology::TRIANGLES;
    };
}

static Mesh ProcessAiMesh(const aiMesh *assimpMesh) {
    const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < assimpMesh->mNumVertices; ++i) {
        const aiVector3D pos = assimpMesh->mVertices[i];
        const aiVector3D normal =
            assimpMesh->HasNormals() ? assimpMesh->mNormals[i] : aiZeroVector;
        const aiVector3D uv = assimpMesh->HasTextureCoords(0)
                                  ? assimpMesh->mTextureCoords[0][i]
                                  : aiZeroVector;
        const aiVector3D tangent = assimpMesh->HasTangentsAndBitangents()
                                       ? assimpMesh->mTangents[i]
                                       : aiZeroVector;
        const aiVector3D bi_tangent = assimpMesh->HasTangentsAndBitangents()
                                          ? assimpMesh->mBitangents[i]
                                          : aiZeroVector;
        vertices.push_back(
            ConstructVertex(pos, uv, normal, tangent, bi_tangent));
    }
    for (uint32_t i = 0; i < assimpMesh->mNumFaces; ++i) {
        const aiFace &face = assimpMesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    Mesh mesh(vertices, indices);
    mesh.SetTopology(ConvertAssimpPrimitive(
        static_cast<aiPrimitiveType>(assimpMesh->mPrimitiveTypes)));
    mesh.SetMaterialIndex(assimpMesh->mMaterialIndex);
    return mesh;
}

static inline TextureWrap ConvertAssimpMapMode(aiTextureMapMode mode) {
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
            SD_CORE_WARN("[ConvertAssimpMapMode] invalid wrap mode: {}!", mode);
            return TextureWrap::REPEAT;
    }
}

static inline MaterialType ConvertAssimpTextureType(aiTextureType textureType) {
    switch (textureType) {
        case aiTextureType_DIFFUSE:
            return MaterialType::DIFFUSE;
        case aiTextureType_AMBIENT:
            return MaterialType::AMBIENT;
        case aiTextureType_SPECULAR:
            return MaterialType::SPECULAR;
        case aiTextureType_EMISSIVE:
            return MaterialType::EMISSIVE;
        case aiTextureType_HEIGHT:
            return MaterialType::HEIGHT;
        case aiTextureType_NORMALS:
            return MaterialType::NORMALS;
        case aiTextureType_SHININESS:
            return MaterialType::SHININESS;
        default:
            SD_CORE_WARN("[ConvertAssimpTextureType] invalid texture type!");
            return MaterialType::NONE;
    }
}

struct MaterialSpec {
    Ref<Bitmap> image;
    MaterialType type;
    TextureSpec texture_spec;
    MaterialSpec(const Ref<Bitmap> &image, const MaterialType &type,
                 const TextureSpec &texture_spec)
        : image(image), type(type), texture_spec(texture_spec) {}
};

static std::vector<MaterialSpec> processAiMaterials(
    AssetManager *manager, const std::filesystem::path &directory,
    const aiMaterial *assimpMaterial) {
    std::vector<MaterialSpec> specs;
    for (int type = 1; type < aiTextureType_SHININESS; ++type) {
        uint32_t count = assimpMaterial->GetTextureCount(aiTextureType(type));
        if (count < 1) {
            continue;
        } else if (count > 1) {
            SD_CORE_WARN(
                "[processAiMaterial] Cannot handle multiple texture of same "
                "type!");
        }

        aiString texturePath;
        aiTextureMapMode map_mode = aiTextureMapMode::aiTextureMapMode_Wrap;
        if (assimpMaterial->GetTexture(aiTextureType(type), 0, &texturePath,
                                       nullptr, nullptr, nullptr, nullptr,
                                       &map_mode) != AI_SUCCESS) {
            SD_CORE_ERROR("[processAiMaterial] Assimp GetTexture error!");
            continue;
        }
        auto image = manager->LoadAndGet<Bitmap>(directory / texturePath.C_Str());
        specs.emplace_back(
            image, ConvertAssimpTextureType(aiTextureType(type)),
            TextureSpec(1, TextureType::TEX_2D,
                        image->HasAlpha() ? DataFormat::RGBA : DataFormat::RGB,
                        DataFormatType::UBYTE, ConvertAssimpMapMode(map_mode),
                        TextureMagFilter::LINEAR,
                        TextureMinFilter::LINEAR_LINEAR));
    }
    return specs;
}

static void processNode(const aiScene *scene, const aiNode *node,
                        Ref<Model> &model) {
    if (node == nullptr) return;
    for (uint32_t i = 0; i < node->mNumChildren; ++i) {
        const aiNode *child = node->mChildren[i];
        for (uint32_t j = 0; j < child->mNumMeshes; ++j) {
            uint32_t id = child->mMeshes[j];
            model->AddMesh(ProcessAiMesh(scene->mMeshes[id]));
        }
        processNode(scene, child, model);
    }
}

Ref<void> ModelLoader::LoadAsset(const std::string &path) {
    Ref<Model> model = CreateRef<Model>();
    SD_CORE_TRACE("Loading model form: {}...", path);

    Assimp::Importer importer;
    uint32_t importFlags = aiProcess_Triangulate | aiProcess_FlipUVs;
    const aiScene *scene = importer.ReadFile(path, importFlags);
    if (scene == nullptr) {
        SD_CORE_ERROR("Model loading failed: {}", importer.GetErrorString());
        return model;
    }

    processNode(scene, scene->mRootNode, model);
    std::filesystem::path directory = std::filesystem::path(path).parent_path();
    ThreadPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<std::vector<MaterialSpec>>> results;
    for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
        results.emplace_back(pool.Queue(processAiMaterials, &Manager(),
                                        directory, scene->mMaterials[i]));
    }
    for (auto &&result : results) {
        Material material;
        for (auto &&spec : result.get()) {
            auto texture = Texture::Create(
                spec.image->Width(), spec.image->Height(), spec.texture_spec);
            texture->SetPixels(0, 0, 0, spec.image->Width(),
                               spec.image->Height(), 1, spec.image->Data());
            material.SetTexture(spec.type, texture);
        }
        model->AddMaterial(material);
    }

    return model;
}

}  // namespace SD
