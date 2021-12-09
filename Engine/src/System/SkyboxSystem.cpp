#include "System/SkyboxSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "ECS/Scene.hpp"

namespace SD {

SkyboxSystem::SkyboxSystem() : System("SkyboxSystem") {
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
    layout.Push(BufferLayoutType::FLOAT3);
    auto vbo = VertexBuffer::Create(skyboxVertices, sizeof(skyboxVertices),
                                    BufferIOType::STATIC);
    auto ibo = IndexBuffer::Create(skyboxIndices, 36, BufferIOType::STATIC);
    m_skybox->AddVertexBuffer(vbo, layout);
    m_skybox->SetIndexBuffer(ibo);
}

void SkyboxSystem::OnInit() {
    m_skyboxShader = ShaderLibrary::Instance().Load("shaders/skybox.glsl");
}

void SkyboxSystem::OnPush() {}

void SkyboxSystem::OnPop() {}

void SkyboxSystem::OnRender() {
    glm::vec3 pos = renderer->GetCamera()->GetWorldPosition();
    glm::mat4 projection = renderer->GetCamera()->GetViewPorjection() *
                           glm::translate(glm::mat4(1.0f), pos);
    m_skyboxShader->SetMat4("u_projection", projection);

    m_skyboxShader->Bind();
    Device::instance().SetDepthfunc(DepthFunc::LESS_EQUAL);
    Device::instance().SetCullFace(Face::FRONT);

    auto skyboxView = renderer->GetScene()->view<SkyboxComponent>();
    auto iter = skyboxView.begin();
    if (iter != skyboxView.end()) {
        auto &skybox = skyboxView.get<SkyboxComponent>(*iter);
        // check if all face are valid
        if (!skybox.skybox.Valid()) {
            for (CubeMapFace face = static_cast<CubeMapFace>(0);
                 face < CubeMapFace::NUMS; ++face) {
                // load invalid face
                if (!skybox.skybox.Valid(face)) {
                    auto bitmap =
                        asset->Get<Bitmap>(skybox.id[static_cast<int>(face)]);
                    if (bitmap) {
                        skybox.skybox.SetFace(face, *bitmap);
                    }
                }
            }
        }
        m_skyboxShader->SetTexture("skybox", skybox.skybox.GetTexture());
    }

    renderer->SetRenderTarget(renderer->GetDefaultTarget());
    renderer->Submit(*m_skybox, MeshTopology::TRIANGLES,
                     m_skybox->GetIndexBuffer()->GetCount(), 0);
    Device::instance().SetCullFace(Face::BACK);
    Device::instance().SetDepthfunc(DepthFunc::LESS);
}

}  // namespace SD
