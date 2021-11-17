#include "Renderer/System/SkyboxSystem.hpp"
#include "Renderer/Renderer.hpp"

namespace SD {

SkyboxSystem::SkyboxSystem(RenderTarget *target)
    : System("Skybox"), m_target(target) {
    const float skyboxVertices[] = {
        // front
        -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
        // back
        -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0};

    const uint32_t skyboxIndices[] = {// front
                                      0, 1, 2, 2, 3, 0,
                                      // right
                                      1, 5, 6, 6, 2, 1,
                                      // back
                                      7, 6, 5, 5, 4, 7,
                                      // left
                                      4, 0, 3, 3, 7, 4,
                                      // bottom
                                      4, 5, 1, 1, 0, 4,
                                      // top
                                      3, 2, 6, 6, 7, 3};
    m_skybox = VertexArray::Create();
    VertexBufferLayout layout;
    layout.Push(BufferDataType::FLOAT, 3);
    auto vbo = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices),
                                    BufferIOType::STATIC);
    auto ibo = IndexBuffer::Create(skyboxIndices, 36, BufferIOType::STATIC);
    m_skybox->AddVertexBuffer(vbo, layout);
    m_skybox->SetIndexBuffer(ibo);

    m_skyboxShader = ShaderLibrary::Instance().Load("shaders/skybox.glsl");
}

void SkyboxSystem::OnPush() {}

void SkyboxSystem::OnPop() {}

void SkyboxSystem::OnRender() {
    glm::vec3 pos = renderer->GetCamera()->GetWorldPosition();
    m_skyboxShader->SetMat4("u_model", glm::translate(glm::mat4(1.0f), pos));

    m_skyboxShader->Bind();
    renderer->GetDevice().SetDepthfunc(DepthFunc::LESS_EQUAL);
    renderer->GetDevice().SetCullFace(Face::FRONT);

    renderer->SetRenderTarget(*m_target);
    renderer->Submit(*m_skybox, MeshTopology::TRIANGLES,
                     m_skybox->GetIndexBuffer()->GetCount(), 0);
    renderer->GetDevice().SetCullFace(Face::BACK);
    renderer->GetDevice().SetDepthfunc(DepthFunc::LESS);
}

}  // namespace SD
