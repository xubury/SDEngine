#include "Renderer/Renderer3D.hpp"

namespace SD {

struct Renderer3DData {
    Ref<UniformBuffer> shadow_UBO;
    Ref<VertexArray> mesh_vao;
    Ref<Texture> default_diffuse;
    Ref<Texture> default_ambient;

    size_t mesh_draw_calls{0};
    size_t mesh_vertex_cnt{0};
};

static Renderer3DData s_mesh_data;

void Renderer3D::Init()
{
    s_mesh_data.shadow_UBO = UniformBuffer::Create(nullptr, sizeof(ShadowData),
                                                   BufferIOType::Dynamic);
    s_mesh_data.mesh_vao = VertexArray::Create();
    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::Float3);
    layout.Push(BufferLayoutType::Float2);
    layout.Push(BufferLayoutType::Float3);
    layout.Push(BufferLayoutType::Float3);
    layout.Push(BufferLayoutType::Float3);
    s_mesh_data.mesh_vao->AddBufferLayout(layout);

    s_mesh_data.default_ambient = Texture::Create(
        1, 1, 0, MultiSampleLevel::None, TextureType::Normal, DataFormat::RGB8,
        TextureWrap::Repeat, TextureMinFilter::Linear, TextureMagFilter::Linear,
        MipmapMode::None);
    s_mesh_data.default_diffuse = Texture::Create(
        1, 1, 0, MultiSampleLevel::None, TextureType::Normal, DataFormat::RGB8,
        TextureWrap::Repeat, TextureMinFilter::Linear, TextureMagFilter::Linear,
        MipmapMode::None);
    const uint8_t color[3] = {0xff, 0xff, 0xff};
    s_mesh_data.default_ambient->SetPixels(0, 0, 0, 1, 1, 1, color);
    s_mesh_data.default_diffuse->SetPixels(0, 0, 0, 1, 1, 1, color);
}

void Renderer3D::Reset()
{
    s_mesh_data.mesh_draw_calls = 0;
    s_mesh_data.mesh_vertex_cnt = 0;
}

std::string Renderer3D::GetDebugInfo()
{
    return fmt::format("Mesh: total draw calls:{}, total vertex counts:{}.\n",
                       s_mesh_data.mesh_draw_calls,
                       s_mesh_data.mesh_vertex_cnt);
}

void Renderer3D::DrawMesh(const Shader& shader, const Mesh& mesh)
{
    s_device->SetPolygonMode(mesh.GetPolygonMode(), Face::Both);
    s_mesh_data.mesh_vao->BindVertexBuffer(*mesh.GetVertexBuffer(), 0);
    s_mesh_data.mesh_vao->BindIndexBuffer(*mesh.GetIndexBuffer());
    Submit(shader, *s_mesh_data.mesh_vao, mesh.GetTopology(),
           mesh.GetIndexBuffer()->GetCount(), 0);

    ++s_mesh_data.mesh_draw_calls;
    s_mesh_data.mesh_vertex_cnt += mesh.GetVertices().size();
}

void Renderer3D::SetMaterial(Shader& shader, const Material& material)
{
    const Texture* diffuse = material.GetTexture(MaterialType::Diffuse);
    const Texture* ambient = material.GetTexture(MaterialType::Ambient);
    const Texture* emissive = material.GetTexture(MaterialType::Emissive);
    const Texture* default_ambient = s_mesh_data.default_ambient.get();
    const Texture* default_diffuse = s_mesh_data.default_diffuse.get();
    shader.GetParam("u_material.diffuse")
        ->SetAsTexture(diffuse ? diffuse : default_diffuse);
    shader.GetParam("u_material.ambient")
        ->SetAsTexture(ambient ? ambient : default_ambient);
    shader.GetParam("u_material.specular")
        ->SetAsTexture(material.GetTexture(MaterialType::Specular));
    shader.GetParam("u_material.emissive")->SetAsTexture(emissive);
    shader.GetParam("u_material.normal")
        ->SetAsTexture(material.GetTexture(MaterialType::Normal));
    // base color
    shader.GetParam("u_material.ambient_color")
        ->SetAsVec3(&material.GetAmbientColor()[0]);
    shader.GetParam("u_material.diffuse_color")
        ->SetAsVec3(&material.GetDiffuseColor()[0]);
    shader.GetParam("u_material.emissive_color")
        ->SetAsVec3(&material.GetEmissiveColor()[0]);
}

void Renderer3D::SetCascadeShadow(const CascadeShadow& shadow)
{
    auto& pv = shadow.GetLevelProjectionView();
    s_mesh_data.shadow_UBO->UpdateData(pv.data(), sizeof(Matrix4f) * pv.size());
}

void Renderer3D::BindCascadeShadow(Shader& shader)
{
    shader.SetUniformBuffer("ShadowData", *s_mesh_data.shadow_UBO);
}

}  // namespace SD
