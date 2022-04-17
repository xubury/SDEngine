#include "Renderer/SkyboxRenderer.hpp"
#include "Renderer/Renderer.hpp"
#include "Resource/Resource.hpp"
#include "Locator/Locator.hpp"

namespace SD {

static ShaderHandle s_skybox_shader;
static Ref<Texture> s_skybox;

void SkyboxRenderer::Init()
{
    auto& shaders = Locator<ShaderCache>::Value();
    auto& cache = Locator<ImageCache>::Value();

    s_skybox_shader =
        shaders.Load("shader/skybox", "assets/shaders/skybox.vert.glsl",
                     "assets/shaders/skybox.frag.glsl");
    std::array<std::string, 6> pathes = {
        "assets/skybox/right.jpg", "assets/skybox/left.jpg",
        "assets/skybox/top.jpg",   "assets/skybox/bottom.jpg",
        "assets/skybox/front.jpg", "assets/skybox/back.jpg"};
    std::array<ByteImage*, 6> images;
    for (size_t i = 0; i < pathes.size(); ++i) {
        images[i] = &cache.Load(pathes[i], pathes[i]).Get();
    }
    s_skybox = Texture::CreateCube(images);
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
