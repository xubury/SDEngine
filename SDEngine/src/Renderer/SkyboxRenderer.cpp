#include "Renderer/SkyboxRenderer.hpp"
#include "Renderer/Renderer.hpp"

#include "Loader/ShaderLoader.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

Ref<Shader> SkyboxRenderer::s_skybox_shader;
Ref<Texture> SkyboxRenderer::s_skybox;

void SkyboxRenderer::Init()
{
    s_skybox_shader = ShaderLoader::LoadShader(
        "assets/shaders/skybox.vert.glsl", "assets/shaders/skybox.frag.glsl");
    s_skybox = TextureLoader::LoadTextureCube(
        {"assets/skybox/right.jpg", "assets/skybox/left.jpg",
         "assets/skybox/top.jpg", "assets/skybox/bottom.jpg",
         "assets/skybox/front.jpg", "assets/skybox/back.jpg"});
}

void SkyboxRenderer::Render(const Camera& camera)
{
    int index = 0;
    RenderOperation op;
    op.depth_func = DepthFunc::LessEqual;
    Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1, op});

    Vector3f pos = camera.GetWorldPosition();
    Matrix4f projection =
        camera.GetViewPorjection() * glm::translate(Matrix4f(1.0f), pos);
    s_skybox_shader->GetParam("u_projection")->SetAsMat4(&projection[0][0]);

    s_skybox_shader->GetParam("u_skybox")->SetAsTexture(s_skybox.get());

    Renderer::DrawNDCBox(*s_skybox_shader);

    Renderer::EndRenderSubpass();
}

}  // namespace SD