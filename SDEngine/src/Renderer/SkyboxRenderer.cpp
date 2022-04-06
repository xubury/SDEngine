#include "Renderer/SkyboxRenderer.hpp"
#include "Renderer/Renderer.hpp"

#include "Loader/ShaderLoader.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

SkyboxRenderer::SkyboxRenderer()
{
    m_skybox_shader = ShaderLoader::LoadShader(
        "assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
    m_skybox = TextureLoader::LoadTextureCube(
        {"assets/skybox/right.jpg", "assets/skybox/left.jpg",
         "assets/skybox/top.jpg", "assets/skybox/bottom.jpg",
         "assets/skybox/front.jpg", "assets/skybox/back.jpg"});
}

void SkyboxRenderer::Render()
{
    int index = 0;
    RenderOperation op;
    op.depth_func = DepthFunc::LessEqual;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1, op});

    Camera *cam = Renderer::GetCamera();
    Vector3f pos = cam->GetWorldPosition();
    Matrix4f projection =
        cam->GetViewPorjection() * glm::translate(Matrix4f(1.0f), pos);
    m_skybox_shader->GetParam("u_projection")->SetAsMat4(&projection[0][0]);

    if (m_skybox) {
        m_skybox_shader->GetParam("u_skybox")->SetAsTexture(m_skybox.get());
    }

    Renderer::DrawNDCBox(*m_skybox_shader);

    Renderer::EndRenderSubpass();
}

}  // namespace SD
