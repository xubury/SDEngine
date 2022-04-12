#include "Loader/ModelLoader.hpp"
#include "Resource/ResourceManager.hpp"
#include "Graphics/Image.hpp"
#include "Utility/Math.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace SD {

static inline Matrix4f ConvertAssimpMatrix(const aiMatrix4x4 &from)
{
    Matrix4f to;
    // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1;
    to[1][0] = from.a2;
    to[2][0] = from.a3;
    to[3][0] = from.a4;
    to[0][1] = from.b1;
    to[1][1] = from.b2;
    to[2][1] = from.b3;
    to[3][1] = from.b4;
    to[0][2] = from.c1;
    to[1][2] = from.c2;
    to[2][2] = from.c3;
    to[3][2] = from.c4;
    to[0][3] = from.d1;
    to[1][3] = from.d2;
    to[2][3] = from.d3;
    to[3][3] = from.d4;
    return to;
}

static Vertex ConstructVertex(const aiVector3D &pos, const aiVector3D &uv,
                              const aiVector3D &normal,
                              const aiVector3D &tangent,
                              const aiVector3D &bi_tangent)
{
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

static MeshTopology ConvertAssimpPrimitive(aiPrimitiveType type)
{
    switch (type) {
        case aiPrimitiveType_LINE:
            return MeshTopology::Lines;
        case aiPrimitiveType_POINT:
            return MeshTopology::Points;
        case aiPrimitiveType_TRIANGLE:
            return MeshTopology::Triangles;
        case aiPrimitiveType_POLYGON:
            return MeshTopology::Quads;
        default:
            SD_CORE_WARN("[ConvertAssimpPrimitive] Unknown mesh topology: {}!",
                         type);
            return MeshTopology::Triangles;
    };
}

static void ProcessAiMesh(const aiMesh *assimpMesh, Model &model)
{
    const aiVector3D aiZeroVector(0.0f, 0.0f, 0.0f);
    std::vector<Vertex> vertices(assimpMesh->mNumVertices);
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
        vertices[i] = ConstructVertex(pos, uv, normal, tangent, bi_tangent);
    }
    for (uint32_t i = 0; i < assimpMesh->mNumFaces; ++i) {
        const aiFace &face = assimpMesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    model.AddMesh(Mesh(vertices, indices,
                       ConvertAssimpPrimitive(static_cast<aiPrimitiveType>(
                           assimpMesh->mPrimitiveTypes))));
}

static inline TextureWrap ConvertAssimpMapMode(aiTextureMapMode mode)
{
    switch (mode) {
        case aiTextureMapMode_Clamp:
            return TextureWrap::Edge;
        case aiTextureMapMode_Decal:
            return TextureWrap::Border;
        case aiTextureMapMode_Mirror:
            return TextureWrap::Mirror;
        case aiTextureMapMode_Wrap:
            return TextureWrap::Repeat;
        default:
            SD_CORE_WARN("[ConvertAssimpMapMode] invalid wrap mode: {}!", mode);
            return TextureWrap::Repeat;
    }
}

static inline MaterialType ConvertAssimpTextureType(aiTextureType textureType)
{
    switch (textureType) {
        case aiTextureType_DIFFUSE:
            return MaterialType::Diffuse;
        case aiTextureType_AMBIENT:
            return MaterialType::Ambient;
        case aiTextureType_SPECULAR:
            return MaterialType::Specular;
        case aiTextureType_EMISSIVE:
            return MaterialType::Emissive;
        case aiTextureType_HEIGHT:
            return MaterialType::Normal;
        case aiTextureType_SHININESS:
            return MaterialType::Shininess;
        default:
            SD_CORE_WARN("[ConvertAssimpTextureType] invalid texture type!");
            return MaterialType::None;
    }
}

static void ProcessAiMaterial(const std::filesystem::path &directory,
                              const aiMaterial *ai_material, Model &model)
{
    Material material;
    auto &resource = ResourceManager::Get();
    for (int type = aiTextureType_NONE + 1; type < aiTextureType_SHININESS;
         ++type) {
        aiTextureType ai_type = aiTextureType(type);
        uint32_t count = ai_material->GetTextureCount(ai_type);
        if (count < 1) {
            continue;
        }
        else if (count > 1) {
            SD_CORE_WARN(
                "[ProcessAiMaterials] Cannot handle multiple texture of same "
                "type!");
        }

        aiString texture_path;
        aiTextureMapMode ai_map_mode = aiTextureMapMode::aiTextureMapMode_Wrap;
        if (ai_material->GetTexture(ai_type, 0, &texture_path, nullptr, nullptr,
                                    nullptr, nullptr,
                                    &ai_map_mode) != AI_SUCCESS) {
            throw Exception("[ProcessAiMaterial] Assimp GetTexture error!");
        }
        std::string full_path =
            (directory / texture_path.C_Str()).generic_string();
        auto image = resource.LoadResource<ByteImage>(full_path);
        auto texture = Texture::Create(
            image->Width(), image->Height(), 0, MultiSampleLevel::None,
            TextureType::Normal, image->GetDataFormat(),
            ConvertAssimpMapMode(ai_map_mode), TextureMinFilter::Linear,
            TextureMagFilter::Linear, MipmapMode::Linear);
        texture->SetPixels(0, 0, 0, image->Width(), image->Height(), 1,
                           image->Data());
        material.SetTexture(ConvertAssimpTextureType(ai_type), texture);
    }
    model.AddMaterial(std::move(material));
}

static void ProcessNode(const aiScene *scene, const aiNode *ai_node,
                        Model &model, ModelNode *parent)
{
    if (ai_node == nullptr) return;

    ModelNode *node = new ModelNode(
        ai_node->mName.C_Str(), ConvertAssimpMatrix(ai_node->mTransformation));
    if (parent) {
        parent->AddChild(node);
    }
    else {
        model.SetRootNode(node);
    }
    for (uint32_t i = 0; i < ai_node->mNumMeshes; ++i) {
        uint32_t mesh_id = ai_node->mMeshes[i];
        uint32_t mat_id = scene->mMeshes[mesh_id]->mMaterialIndex;
        node->AddMesh(mesh_id);
        node->AddMaterial(mat_id);
    }

    for (uint32_t i = 0; i < ai_node->mNumChildren; ++i) {
        const aiNode *ai_child = ai_node->mChildren[i];
        ProcessNode(scene, ai_child, model, node);
    }
}

Ref<Model> ModelLoader::LoadFromFile(const std::string &path)
{
    Ref<Model> model;
    SD_CORE_TRACE("Loading model form: {}...", path);

    Assimp::Importer importer;
    uint32_t import_flags =
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
    const aiScene *scene = importer.ReadFile(path, import_flags);
    if (scene == nullptr) {
        throw FileException(path, fmt::format("Model loading failed: {}",
                                              importer.GetErrorString()));
    }
    else {
        model = CreateRef<Model>();
        // Process materials
        std::filesystem::path directory =
            std::filesystem::path(path).parent_path();
        for (uint32_t i = 0; i < scene->mNumMaterials; ++i) {
            ProcessAiMaterial(directory, scene->mMaterials[i], *model);
        }

        // Process meshes
        for (uint32_t i = 0; i < scene->mNumMeshes; ++i) {
            ProcessAiMesh(scene->mMeshes[i], *model);
        }

        // Process node
        ProcessNode(scene, scene->mRootNode, *model, nullptr);
    }
    return model;
}

}  // namespace SD
