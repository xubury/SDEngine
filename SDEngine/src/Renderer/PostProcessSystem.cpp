#include "Renderer/PostProcessSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Loader/ShaderLoader.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Core/Application.hpp"

namespace SD {

PostProcessSystem::PostProcessSystem(int32_t width, int32_t height)
    : ECSSystem("PostProcessSystem"),
      m_width(width),
      m_height(height),
      m_blur_result(nullptr),
      m_bloom_threshold(1.0),
      m_bloom_soft_threshold(0.8),
      m_is_bloom(true),
      m_exposure(1.2),
      m_gamma_correction(1.2)
{
}

void PostProcessSystem::OnInit()
{
    ECSSystem::OnInit();

    auto &dispatcher = GetEventDispatcher();
    auto &settings = Application::GetApp().GetSettings();
    m_is_bloom = settings.GetBoolean("post process", "bloom", m_is_bloom);
    m_bloom_threshold =
        settings.GetFloat("post process", "bloom threshold", m_bloom_threshold);
    m_bloom_soft_threshold = settings.GetFloat(
        "post process", "bloom soft threshold", m_bloom_soft_threshold);
    m_exposure = settings.GetFloat("post process", "exposure", m_exposure);
    m_gamma_correction = settings.GetFloat("post process", "gamma correction",
                                           m_gamma_correction);
    m_size_handler = dispatcher.Register(this, &PostProcessSystem::OnSizeEvent);

    m_blur_shader = ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                             "assets/shaders/blur.frag.glsl");
    m_post_shader =
        ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                 "assets/shaders/post_process.frag.glsl");
    m_bloom_shader = ShaderLoader::LoadShader("assets/shaders/bloom.comp.glsl");
    InitBuffers();
}

void PostProcessSystem::OnDestroy()
{
    auto &dispatcher = GetEventDispatcher();
    auto &settings = Application::GetApp().GetSettings();
    settings.SetBoolean("post process", "bloom", m_is_bloom);
    settings.SetFloat("post process", "bloom threshold", m_bloom_threshold);
    settings.SetFloat("post process", "bloom soft threshold",
                      m_bloom_soft_threshold);
    settings.SetFloat("post process", "exposure", m_exposure);
    settings.SetFloat("post process", "gamma correction", m_gamma_correction);
    dispatcher.RemoveHandler(m_size_handler);
    ECSSystem::OnDestroy();
}

void PostProcessSystem::InitBuffers()
{
    for (int i = 0; i < 2; ++i) {
        m_blur_buffers[i] = Texture::Create(
            m_width, m_height, 0, MultiSampleLevel::None, TextureType::Normal,
            DataFormat::RGBA16F, TextureWrap::Edge, TextureMinFilter::Nearest,
            TextureMagFilter::Nearest, MipmapMode::Linear);
        m_blur_targets[i] = Framebuffer::Create();
        m_blur_targets[i]->Attach(*m_blur_buffers[i], 0, 0);
    }
    m_post_target = Framebuffer::Create();
    m_post_buffer = Texture::Create(
        m_width, m_height, 0, MultiSampleLevel::None, TextureType::Normal,
        DataFormat::RGBA16F, TextureWrap::Edge, TextureMinFilter::Nearest,
        TextureMagFilter::Nearest, MipmapMode::Linear);
    m_post_target->Attach(*m_post_buffer, 0, 0);

    m_downsample_buffer = Texture::Create(
        m_width, m_height, 0, MultiSampleLevel::None, TextureType::Normal,
        DataFormat::RGBA16F, TextureWrap::Edge, TextureMinFilter::Linear,
        TextureMagFilter::Linear, MipmapMode::Linear, 7);
    m_upsample_buffer = Texture::Create(
        m_width, m_height, 0, MultiSampleLevel::None, TextureType::Normal,
        DataFormat::RGBA16F, TextureWrap::Edge, TextureMinFilter::Linear,
        TextureMagFilter::Linear, MipmapMode::Linear,
        m_downsample_buffer->GetMipmapLevels() - 1);
}

void PostProcessSystem::OnImGui()
{
    ImGui::Begin("PostProcess System");
    {
        ImGui::SliderFloat("Exposure", &m_exposure, 0, 10);
        ImGui::SliderFloat("Gamma Correction", &m_gamma_correction, 0.1, 3);
        ImGui::Checkbox("Bloom", &m_is_bloom);
        ImGui::SliderFloat("Bloom Threshold", &m_bloom_threshold, 0, 1.0);
        ImGui::SliderFloat("Bloom Soft Threshold", &m_bloom_soft_threshold,
                           0.01, 1.0f);
        static int base_level = 0;
        static int buffer_index = 0;
        ImGui::SliderInt("Buffer index", &buffer_index, 0, 1);
        Texture *buffer = buffer_index == 0 ? m_downsample_buffer.get()
                                            : m_upsample_buffer.get();
        ImGui::SliderInt("Base level", &base_level, 0,
                         buffer->GetMipmapLevels() - 1);
        buffer->SetBaseLevel(base_level);
        ImGui::DrawTexture(*buffer, ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();
}

void PostProcessSystem::OnRender()
{
    Renderer::BlitToBuffer(0, m_post_target.get(), 0,
                           BufferBitMask::ColorBufferBit);

    if (m_is_bloom) {
        RenderBloom();
    }
    RenderPost();
}

void PostProcessSystem::OnSizeEvent(const RenderSizeEvent &event)
{
    m_width = event.width;
    m_height = event.height;
    InitBuffers();
}

void PostProcessSystem::RenderPost()
{
    int index = 0;
    RenderSubpassInfo info{&index, 1};
    Renderer::BeginRenderSubpass(info);
    m_post_shader->GetParam("u_bloom")->SetAsBool(m_is_bloom);
    m_post_shader->GetParam("u_upsample_buffer")
        ->SetAsTexture(m_upsample_buffer.get());

    m_post_shader->GetParam("u_lighting")->SetAsTexture(m_post_buffer.get());
    m_post_shader->GetParam("u_exposure")->SetAsFloat(m_exposure);

    m_post_shader->GetParam("u_gamma")->SetAsFloat(m_gamma_correction);

    Renderer::DrawNDCQuad(*m_post_shader);

    Renderer::EndRenderSubpass();
}

void PostProcessSystem::Downsample(Texture &src, Texture &dst)
{
    m_bloom_shader->GetParam("u_downsample")->SetAsBool(true);
    m_bloom_shader->GetParam("u_threshold")->SetAsFloat(m_bloom_threshold);
    const float knee = m_bloom_threshold * m_bloom_soft_threshold;
    Vector3f filter;
    filter.x = m_bloom_threshold - knee;
    filter.y = 2.f * knee;
    filter.z = 0.25f / (knee + 1e-4);
    m_bloom_shader->GetParam("u_curve")->SetAsVec3(&filter[0]);
    for (int base_level = 0; base_level < dst.GetMipmapLevels(); ++base_level) {
        if (base_level == 0) {
            m_bloom_shader->GetParam("u_input")->SetAsBool(true);
            m_bloom_shader->GetParam("u_out_image")
                ->SetAsImage(&dst, 0, false, 0, Access::WriteOnly);

            m_bloom_shader->GetParam("u_in_texture")->SetAsTexture(&src);
        }
        else {
            m_bloom_shader->GetParam("u_input")->SetAsBool(false);
            m_bloom_shader->GetParam("u_out_image")
                ->SetAsImage(&dst, base_level, false, 0, Access::WriteOnly);

            m_bloom_shader->GetParam("u_level")->SetAsInt(base_level - 1);
            m_bloom_shader->GetParam("u_in_texture")->SetAsTexture(&dst);
        }
        Renderer::DispatchCompute(*m_bloom_shader, std::ceil(m_width / 13.f),
                                  std::ceil(m_height / 13.f), 1);
    }
}

void PostProcessSystem::Upsample(Texture &src, Texture &dst)
{
    const int max_level = dst.GetMipmapLevels() - 1;
    m_bloom_shader->GetParam("u_downsample")->SetAsBool(false);
    for (int base_level = max_level; base_level >= 0; --base_level) {
        m_bloom_shader->GetParam("u_level")->SetAsInt(base_level + 1);
        if (base_level == max_level) {
            m_bloom_shader->GetParam("u_input")->SetAsBool(true);
            m_bloom_shader->GetParam("u_out_image")
                ->SetAsImage(&dst, base_level, false, 0, Access::WriteOnly);

            m_bloom_shader->GetParam("u_in_texture")->SetAsTexture(&src);
        }
        else {
            m_bloom_shader->GetParam("u_input")->SetAsBool(false);
            m_bloom_shader->GetParam("u_out_image")
                ->SetAsImage(&dst, base_level, false, 0, Access::WriteOnly);

            m_bloom_shader->GetParam("u_down_texture")->SetAsTexture(&src);
            m_bloom_shader->GetParam("u_in_texture")->SetAsTexture(&dst);
        }
        Renderer::DispatchCompute(*m_bloom_shader, std::ceil(m_width / 13.f),
                                  std::ceil(m_height / 13.f), 1);
    }
}

void PostProcessSystem::RenderBloom()
{
    // make sure base level is readable
    m_downsample_buffer->SetBaseLevel(0);
    m_upsample_buffer->SetBaseLevel(0);
    Downsample(*m_post_buffer, *m_downsample_buffer);
    Upsample(*m_downsample_buffer, *m_upsample_buffer);
}

void PostProcessSystem::SetExposure(float exposure) { m_exposure = exposure; }

float PostProcessSystem::GetExposure() { return m_exposure; }

void PostProcessSystem::SetBloom(bool isBloom) { m_is_bloom = isBloom; }

bool PostProcessSystem::GetBloom() { return m_is_bloom; }

void PostProcessSystem::SetGammaCorrection(float gamma)
{
    m_gamma_correction = gamma;
}

float PostProcessSystem::GetGammaCorrection() { return m_gamma_correction; }

}  // namespace SD
