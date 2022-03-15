#include "System/SkyboxSystem.hpp"
#include "ECS/Scene.hpp"
#include "Renderer/Renderer.hpp"

#include "Asset/AssetStorage.hpp"
#include "Loader/ShaderLoader.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

SkyboxSystem::SkyboxSystem() : System("SkyboxSystem") {
    const float skybox_vertices[] = {
        // front
        -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0,
        // back
        -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0};

    const uint32_t skybox_indices[] = {// front
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
    m_box_vao = VertexArray::Create();
    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::FLOAT3);
    auto vbo = VertexBuffer::Create(skybox_vertices, sizeof(skybox_vertices),
                                    BufferIOType::STATIC);
    auto ibo = IndexBuffer::Create(skybox_indices, 36, BufferIOType::STATIC);
    m_box_vao->AddVertexBuffer(vbo, layout);
    m_box_vao->SetIndexBuffer(ibo);
}

void SkyboxSystem::OnInit() {
    System::OnInit();
    m_skybox_shader = ShaderLoader::LoadShader(
        "assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
    m_skybox = TextureLoader::LoadTextureCube(
        {"assets/skybox/right.jpg", "assets/skybox/left.jpg",
         "assets/skybox/top.jpg", "assets/skybox/bottom.jpg",
         "assets/skybox/front.jpg", "assets/skybox/back.jpg"});
}

void SkyboxSystem::OnPush() {}

void SkyboxSystem::OnPop() {}

void SkyboxSystem::OnImGui() {}

void SkyboxSystem::OnRender() {
    int index = 0;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1});
    glm::vec3 pos = scene->GetCamera()->GetWorldPosition();
    glm::mat4 projection = scene->GetCamera()->GetViewPorjection() *
                           glm::translate(glm::mat4(1.0f), pos);
    m_skybox_shader->SetMat4("u_projection", projection);

    device->SetDepthfunc(DepthFunc::LESS_EQUAL);
    if (m_skybox) {
        m_skybox_shader->SetTexture("u_skybox", m_skybox.get());
    }

    Renderer::Submit(*m_skybox_shader, *m_box_vao, MeshTopology::TRIANGLES,
                     m_box_vao->GetIndexBuffer()->GetCount(), 0);
    device->SetDepthfunc(DepthFunc::LESS);
    Renderer::EndRenderSubpass();
}

}  // namespace SD
