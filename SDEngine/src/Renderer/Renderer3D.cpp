#include "Renderer/Renderer3D.hpp"

namespace SD {

struct Renderer3DData {
    Ref<UniformBuffer> shadow_UBO;
    Ref<VertexArray> mesh_vao;

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
    s_device->SetShader(&shader);
    s_mesh_data.mesh_vao->BindVertexBuffer(*mesh.GetVertexBuffer(), 0);
    s_mesh_data.mesh_vao->BindIndexBuffer(*mesh.GetIndexBuffer());
    Submit(*s_mesh_data.mesh_vao, mesh.GetTopology(),
           mesh.GetIndexBuffer()->GetCount(), 0);

    ++s_mesh_data.mesh_draw_calls;
    s_mesh_data.mesh_vertex_cnt += mesh.GetVertices().size();
}

void Renderer3D::SetMaterial(Shader& shader, const Material& material)
{
    shader.GetParam("u_material.diffuse")
        ->SetAsTexture(material.GetTexture(MaterialType::Diffuse));
    shader.GetParam("u_material.specular")
        ->SetAsTexture(material.GetTexture(MaterialType::Specular));
    shader.GetParam("u_material.ambient")
        ->SetAsTexture(material.GetTexture(MaterialType::Ambient));
    shader.GetParam("u_material.emissive")
        ->SetAsTexture(material.GetTexture(MaterialType::Emissive));
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

void Renderer3D::SetShadowCaster(Shader& shader, const CascadeShadow& shadow)
{
    auto& pv = shadow.GetLevelProjectionView();
    s_mesh_data.shadow_UBO->UpdateData(pv.data(), sizeof(Matrix4f) * pv.size());

    shader.SetUniformBuffer("ShadowData", *s_mesh_data.shadow_UBO);
}

}  // namespace SD
