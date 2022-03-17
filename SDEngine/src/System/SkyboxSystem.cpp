#include "System/SkyboxSystem.hpp"
#include "ECS/Scene.hpp"
#include "Renderer/Renderer.hpp"

#include "Asset/AssetStorage.hpp"
#include "Loader/ShaderLoader.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

SkyboxSystem::SkyboxSystem() : System("SkyboxSystem") {}

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

    device->SetShader(m_skybox_shader.get());
    Renderer::DrawNDCBox();
    device->SetDepthfunc(DepthFunc::LESS);
    Renderer::EndRenderSubpass();
}

}  // namespace SD
