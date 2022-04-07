#include "Renderer/PostProcessRenderer.hpp"
#include "Renderer/Renderer.hpp"
#include "Loader/ShaderLoader.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Core/Application.hpp"

namespace SD {

struct PostProcessData {
    int width;
    int height;

    Ref<Shader> hdr_shader;
    Ref<Shader> bloom_shader;

    Ref<Framebuffer> post_target;
    Ref<Texture> post_buffer;

    Ref<Texture> upsample_buffer;
    Ref<Texture> downsample_buffer;
    float bloom_threshold{1.0};
    float bloom_soft_threshold{0.8};

    bool is_bloom{true};
    float exposure{1.0};
    float gamma_correction{1.5};
};

static PostProcessData s_data;

void PostProcessRenderer::Init(int32_t width, int32_t height)
{
    auto &settings = Application::GetApp().GetSettings();
    s_data.width = width;
    s_data.height = height;
    s_data.is_bloom =
        settings.GetBoolean("post process", "bloom", s_data.is_bloom);
    s_data.bloom_threshold = settings.GetFloat(
        "post process", "bloom threshold", s_data.bloom_threshold);
    s_data.bloom_soft_threshold = settings.GetFloat(
        "post process", "bloom soft threshold", s_data.bloom_soft_threshold);
    s_data.exposure =
        settings.GetFloat("post process", "exposure", s_data.exposure);
    s_data.gamma_correction = settings.GetFloat(
        "post process", "gamma correction", s_data.gamma_correction);

    s_data.hdr_shader = ShaderLoader::LoadShader(
        "assets/shaders/quad.vert.glsl", "assets/shaders/hdr.frag.glsl");
    s_data.bloom_shader =
        ShaderLoader::LoadShader("assets/shaders/bloom.comp.glsl");
    s_data.post_target = Framebuffer::Create();
    InitBuffers();
}

void PostProcessRenderer::Shutdown()
{
    auto &settings = Application::GetApp().GetSettings();
    settings.SetBoolean("post process", "bloom", s_data.is_bloom);
    settings.SetFloat("post process", "bloom threshold",
                      s_data.bloom_threshold);
    settings.SetFloat("post process", "bloom soft threshold",
                      s_data.bloom_soft_threshold);
    settings.SetFloat("post process", "exposure", s_data.exposure);
    settings.SetFloat("post process", "gamma correction",
                      s_data.gamma_correction);
}

void PostProcessRenderer::SetRenderSize(int32_t width, int32_t height)
{
    s_data.width = width;
    s_data.height = height;
    InitBuffers();
}

void PostProcessRenderer::InitBuffers()
{
    s_data.post_buffer =
        Texture::Create(s_data.width, s_data.height, 0, MultiSampleLevel::None,
                        TextureType::Normal, DataFormat::RGBA16F,
                        TextureWrap::Edge, TextureMinFilter::Nearest,
                        TextureMagFilter::Nearest, MipmapMode::Linear);
    s_data.post_target->Attach(*s_data.post_buffer, 0, 0);

    s_data.downsample_buffer =
        Texture::Create(s_data.width, s_data.height, 0, MultiSampleLevel::None,
                        TextureType::Normal, DataFormat::RGBA16F,
                        TextureWrap::Edge, TextureMinFilter::Linear,
                        TextureMagFilter::Linear, MipmapMode::Linear, 7);
    s_data.upsample_buffer = Texture::Create(
        s_data.width, s_data.height, 0, MultiSampleLevel::None,
        TextureType::Normal, DataFormat::RGBA16F, TextureWrap::Edge,
        TextureMinFilter::Linear, TextureMagFilter::Linear, MipmapMode::Linear,
        s_data.downsample_buffer->GetMipmapLevels() - 1);
}

void PostProcessRenderer::ImGui()
{
    ImGui::SliderFloat("Exposure", &s_data.exposure, 0, 10);
    ImGui::SliderFloat("Gamma Correction", &s_data.gamma_correction, 0.1, 3);
    ImGui::Checkbox("Bloom", &s_data.is_bloom);
    ImGui::SliderFloat("Bloom Threshold", &s_data.bloom_threshold, 0, 1.0);
    ImGui::SliderFloat("Bloom Soft Threshold", &s_data.bloom_soft_threshold,
                       0.01, 1.0f);
    static int base_level = 0;
    static int buffer_index = 0;
    ImGui::SliderInt("Buffer index", &buffer_index, 0, 1);
    Texture *buffer = buffer_index == 0 ? s_data.downsample_buffer.get()
                                        : s_data.upsample_buffer.get();
    ImGui::SliderInt("Base level", &base_level, 0,
                     buffer->GetMipmapLevels() - 1);
    buffer->SetBaseLevel(base_level);
    ImGui::DrawTexture(*buffer, ImVec2(0, 1), ImVec2(1, 0));
}

void PostProcessRenderer::Render()
{
    Renderer::BlitToBuffer(0, s_data.post_target.get(), 0,
                           BufferBitMask::ColorBufferBit);

    if (s_data.is_bloom) {
        RenderBloom();
    }
    RenderPost();
}

void PostProcessRenderer::RenderPost()
{
    int index = 0;
    RenderSubpassInfo info{&index, 1};
    Renderer::BeginRenderSubpass(info);
    s_data.hdr_shader->GetParam("u_bloom")->SetAsBool(s_data.is_bloom);
    s_data.hdr_shader->GetParam("u_upsample_buffer")
        ->SetAsTexture(s_data.upsample_buffer.get());

    s_data.hdr_shader->GetParam("u_lighting")
        ->SetAsTexture(s_data.post_buffer.get());
    s_data.hdr_shader->GetParam("u_exposure")->SetAsFloat(s_data.exposure);

    s_data.hdr_shader->GetParam("u_gamma")->SetAsFloat(s_data.gamma_correction);

    Renderer::DrawNDCQuad(*s_data.hdr_shader);

    Renderer::EndRenderSubpass();
}

void PostProcessRenderer::Downsample(Texture &src, Texture &dst)
{
    s_data.bloom_shader->GetParam("u_downsample")->SetAsBool(true);
    s_data.bloom_shader->GetParam("u_threshold")
        ->SetAsFloat(s_data.bloom_threshold);
    const float knee = s_data.bloom_threshold * s_data.bloom_soft_threshold;
    Vector3f filter;
    filter.x = s_data.bloom_threshold - knee;
    filter.y = 2.f * knee;
    filter.z = 0.25f / (knee + 1e-4);
    s_data.bloom_shader->GetParam("u_curve")->SetAsVec3(&filter[0]);
    for (int base_level = 0; base_level < dst.GetMipmapLevels(); ++base_level) {
        if (base_level == 0) {
            s_data.bloom_shader->GetParam("u_input")->SetAsBool(true);
            s_data.bloom_shader->GetParam("u_out_image")
                ->SetAsImage(&dst, 0, false, 0, Access::WriteOnly);

            s_data.bloom_shader->GetParam("u_in_texture")->SetAsTexture(&src);
        }
        else {
            s_data.bloom_shader->GetParam("u_input")->SetAsBool(false);
            s_data.bloom_shader->GetParam("u_out_image")
                ->SetAsImage(&dst, base_level, false, 0, Access::WriteOnly);

            s_data.bloom_shader->GetParam("u_level")->SetAsInt(base_level - 1);
            s_data.bloom_shader->GetParam("u_in_texture")->SetAsTexture(&dst);
        }
        Renderer::ComputeImage(*s_data.bloom_shader, s_data.width,
                               s_data.height, 1);
    }
}

void PostProcessRenderer::Upsample(Texture &src, Texture &dst)
{
    const int max_level = dst.GetMipmapLevels() - 1;
    s_data.bloom_shader->GetParam("u_downsample")->SetAsBool(false);
    for (int base_level = max_level; base_level >= 0; --base_level) {
        s_data.bloom_shader->GetParam("u_level")->SetAsInt(base_level + 1);
        if (base_level == max_level) {
            s_data.bloom_shader->GetParam("u_input")->SetAsBool(true);
            s_data.bloom_shader->GetParam("u_out_image")
                ->SetAsImage(&dst, base_level, false, 0, Access::WriteOnly);

            s_data.bloom_shader->GetParam("u_in_texture")->SetAsTexture(&src);
        }
        else {
            s_data.bloom_shader->GetParam("u_input")->SetAsBool(false);
            s_data.bloom_shader->GetParam("u_out_image")
                ->SetAsImage(&dst, base_level, false, 0, Access::WriteOnly);

            s_data.bloom_shader->GetParam("u_down_texture")->SetAsTexture(&src);
            s_data.bloom_shader->GetParam("u_in_texture")->SetAsTexture(&dst);
        }
        Renderer::ComputeImage(*s_data.bloom_shader, s_data.width,
                               s_data.height, 1);
    }
}

void PostProcessRenderer::RenderBloom()
{
    // make sure base level is readable
    s_data.downsample_buffer->SetBaseLevel(0);
    s_data.upsample_buffer->SetBaseLevel(0);
    Downsample(*s_data.post_buffer, *s_data.downsample_buffer);
    Upsample(*s_data.downsample_buffer, *s_data.upsample_buffer);
}

}  // namespace SD