#include "Renderer/Renderer3D.hpp"

namespace SD {

Ref<UniformBuffer> Renderer3D::m_shadow_UBO;
Ref<VertexArray> Renderer3D::m_mesh_vao;

void Renderer3D::Init()
{
    m_shadow_UBO = UniformBuffer::Create(nullptr, sizeof(ShadowData),
                                         BufferIOType::Dynamic);
    m_mesh_vao = VertexArray::Create();
    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::Float3);
    layout.Push(BufferLayoutType::Float2);
    layout.Push(BufferLayoutType::Float3);
    layout.Push(BufferLayoutType::Float3);
    layout.Push(BufferLayoutType::Float3);
    m_mesh_vao->AddBufferLayout(layout);
}

void Renderer3D::DrawMesh(const Shader& shader, const Mesh& mesh)
{
    m_device->SetPolygonMode(mesh.GetPolygonMode(), Face::Both);
    m_device->SetShader(&shader);
    m_mesh_vao->BindVertexBuffer(*mesh.GetVertexBuffer(), 0);
    m_mesh_vao->BindIndexBuffer(*mesh.GetIndexBuffer());
    Submit(*m_mesh_vao, mesh.GetTopology(), mesh.GetIndexBuffer()->GetCount(),
           0);
}

void Renderer3D::SetMaterial(Shader& shader, const Material& material)
{
    m_device->SetShader(&shader);

    shader.GetParam("u_material.diffuse")
        ->SetAsTexture(material.GetTexture(MaterialType::DIFFUSE));
    shader.GetParam("u_material.specular")
        ->SetAsTexture(material.GetTexture(MaterialType::SPECULAR));
    shader.GetParam("u_material.ambient")
        ->SetAsTexture(material.GetTexture(MaterialType::AMBIENT));
    shader.GetParam("u_material.emissive")
        ->SetAsTexture(material.GetTexture(MaterialType::EMISSIVE));
}

void Renderer3D::SetShadowCaster(Shader& shader, CascadeShadow& shadow)
{
    auto& pv = shadow.GetLevelProjectionView();
    m_shadow_UBO->UpdateData(pv.data(), sizeof(glm::mat4) * pv.size());

    shader.SetUniformBuffer("ShadowData", *m_shadow_UBO);
}

}  // namespace SD
