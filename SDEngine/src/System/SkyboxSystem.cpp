#include "System/SkyboxSystem.hpp"
#include "ECS/Scene.hpp"
#include "Renderer/Renderer.hpp"

#include "Asset/AssetStorage.hpp"
#include "Loader/ShaderLoader.hpp"
#include "Loader/TextureLoader.hpp"

#include <glm/gtc/type_ptr.hpp>

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
    RenderOperation op;
    op.depth_func = DepthFunc::LessEqual;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1, op});

    glm::vec3 pos = scene->GetCamera()->GetWorldPosition();
    glm::mat4 projection = scene->GetCamera()->GetViewPorjection() *
                           glm::translate(glm::mat4(1.0f), pos);
    m_skybox_shader->GetParam("u_projection")
        ->SetAsMat4(glm::value_ptr(projection));

    if (m_skybox) {
        m_skybox_shader->GetParam("u_skybox")->SetAsTexture(m_skybox.get());
    }

    Renderer::DrawNDCBox(*m_skybox_shader);

    Renderer::EndRenderSubpass();
}

}  // namespace SD
