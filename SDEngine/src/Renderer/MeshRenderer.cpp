
#include "Renderer/MeshRenderer.hpp"

namespace SD {

Ref<UniformBuffer> MeshRenderer::m_shadow_UBO;

void MeshRenderer::Init() {
    m_shadow_UBO = UniformBuffer::Create(nullptr, sizeof(ShadowData),
                                         BufferIOType::DYNAMIC);
}

void MeshRenderer::DrawMesh(const Shader& shader, const Mesh& mesh) {
    m_device->SetPolygonMode(mesh.GetPolygonMode(), Face::BOTH);
    VertexArray* vao = mesh.GetVertexArray();
    SD_CORE_ASSERT(vao, "Invalid mesh!");
    m_device->SetShader(&shader);
    Submit(*vao, mesh.GetTopology(), vao->GetIndexBuffer()->GetCount(), 0);
}

void MeshRenderer::SetMaterial(Shader& shader, const Material& material) {
    m_device->SetShader(&shader);

    shader.SetTexture("u_material.diffuse",
                      material.GetTexture(MaterialType::DIFFUSE));
    shader.SetTexture("u_material.specular",
                      material.GetTexture(MaterialType::SPECULAR));
    shader.SetTexture("u_material.ambient",
                      material.GetTexture(MaterialType::AMBIENT));
    shader.SetTexture("u_material.emissive",
                      material.GetTexture(MaterialType::EMISSIVE));
}

void MeshRenderer::Begin(Shader& shader, Camera& camera) {
    m_camera_data.view = camera.GetView();
    m_camera_data.projection = camera.GetProjection();
    m_camera_UBO->UpdateData(&m_camera_data, sizeof(CameraData));

    SetupShaderUBO(shader);
}

void MeshRenderer::Begin(Light& light, const Transform& light_trans,
                         Camera& camera, Shader& shader) {
    // Framebuffer* fb = light.GetCascadeMap();
    // m_device->SetFramebuffer(fb);
    // m_device->SetViewport(0, 0, fb->GetWidth(), fb->GetHeight());
    m_device->Clear(BufferBitMask::DEPTH_BUFFER_BIT);

    light.ComputeCascadeLightMatrix(light_trans, camera);

    auto& pv = light.GetLevelProjectionView();
    m_shadow_UBO->UpdateData(pv.data(), sizeof(glm::mat4) * pv.size());

    SetupShaderUBO(shader);
}

void MeshRenderer::SetupShaderUBO(Shader& shader) {
    shader.SetUniformBuffer("ShadowData", *m_shadow_UBO);
    shader.SetUniformBuffer("Camera", *m_camera_UBO);
}

void MeshRenderer::End() {}

}  // namespace SD
