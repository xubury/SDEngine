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
      m_is_bloom(true),
      m_bloom_factor(1.0f),
      m_exposure(1.2),
      m_gamma_correction(1.2)
{
}

void PostProcessSystem::OnInit()
{
    ECSSystem::OnInit();
    m_blur_shader = ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                             "assets/shaders/blur.frag.glsl");
    m_post_shader =
        ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                 "assets/shaders/post_process.frag.glsl");
    m_test_shader = ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                             "assets/shaders/test.frag.glsl");
    InitBuffers();
}

void PostProcessSystem::InitBuffers()
{
    for (int i = 0; i < 2; ++i) {
        m_blur_buffers[i] = Texture::Create(
            m_width, m_height, 0, MultiSampleLevel::None, TextureType::Normal,
            DataFormat::RGBA16F, TextureWrap::Edge, TextureMinFilter::Nearest,
            MipmapMode::Linear, TextureMagFilter::Nearest);
        m_blur_targets[i] = Framebuffer::Create();
        m_blur_targets[i]->Attach(*m_blur_buffers[i], 0, 0);
    }
    m_post_target = Framebuffer::Create();
    m_post_buffer = Texture::Create(
        m_width, m_height, 0, MultiSampleLevel::None, TextureType::Normal,
        DataFormat::RGBA16F, TextureWrap::Edge, TextureMinFilter::Nearest,
        MipmapMode::Linear, TextureMagFilter::Nearest);
    m_post_target->Attach(*m_post_buffer, 0, 0);

    m_test_buffer = Texture::Create(
        m_width, m_height, 0, MultiSampleLevel::None, TextureType::Normal,
        DataFormat::RGBA16F, TextureWrap::Edge, TextureMinFilter::Nearest,
        MipmapMode::Linear, TextureMagFilter::Nearest);
    m_test_target = Framebuffer::Create();
    m_test_target->Attach(*m_test_buffer, 0, 0);
}

void PostProcessSystem::OnPush()
{
    auto &dispatcher = GetEventDispatcher();
    auto &settings = Application::GetApp().GetSettings();
    m_is_bloom = settings.GetBoolean("post process", "bloom", m_is_bloom);
    m_bloom_factor =
        settings.GetFloat("post process", "bloom factor", m_bloom_factor);
    m_exposure = settings.GetFloat("post process", "exposure", m_exposure);
    m_gamma_correction = settings.GetFloat("post process", "gamma correction",
                                           m_gamma_correction);
    m_size_handler = dispatcher.Register(this, &PostProcessSystem::OnSizeEvent);
}

void PostProcessSystem::OnPop()
{
    auto &dispatcher = GetEventDispatcher();
    auto &settings = Application::GetApp().GetSettings();
    settings.SetBoolean("post process", "bloom", m_is_bloom);
    settings.SetFloat("post process", "bloom factor", m_bloom_factor);
    settings.SetFloat("post process", "exposure", m_exposure);
    settings.SetFloat("post process", "gamma correction", m_gamma_correction);
    dispatcher.RemoveHandler(m_size_handler);
}

void PostProcessSystem::OnImGui()
{
    ImGui::Begin("PostProcess System");
    {
        ImGui::TextUnformatted("Exposure");
        ImGui::SliderFloat("##Exposure", &m_exposure, 0, 10);
        ImGui::Checkbox("Bloom", &m_is_bloom);
        ImGui::TextUnformatted("Bloom Factor");
        ImGui::SliderFloat("##Bloom Factor", &m_bloom_factor, 0.1, 1);

        ImGui::TextUnformatted("Gamma Correction");
        ImGui::SliderFloat("##Gamma Correction", &m_gamma_correction, 0.1, 3);
    }
    ImGui::End();

    ImGui::Begin("PostProcess Debug");
    {
        // SD_CORE_TRACE("level:{}",
        //               m_test_buffer->GetTexture()->GetMipmapLevels());
        m_test_buffer->GenerateMipmap();
        m_test_buffer->SetBaseLevel(5);
        // m_test_buffer->GetTexture()->SetMaxLevel(10);
        ImGui::DrawTexture(*m_test_buffer);
    }
    ImGui::End();
}

void PostProcessSystem::OnRender()
{
    Renderer::BlitToBuffer(0, m_post_target.get(), 0,
                           BufferBitMask::ColorBufferBit);

    RenderMipmapTest();
    if (m_is_bloom) {
        RenderBlur();
    }
    RenderPost();
}

void PostProcessSystem::OnSizeEvent(const RenderSizeEvent &event)
{
    m_width = event.width;
    m_height = event.height;
    InitBuffers();
}

void PostProcessSystem::RenderBlur()
{
    const int amount = 10;
    bool horizontal = true;
    ShaderParam *horizontal_param = m_blur_shader->GetParam("u_horizontal");
    ShaderParam *image = m_blur_shader->GetParam("u_image");
    for (int i = 0; i < amount; ++i) {
        const int inputId = horizontal;
        const int outputId = !horizontal;
        Renderer::BeginRenderPass(
            RenderPassInfo{m_blur_targets[outputId].get(), m_width, m_height});
        m_blur_result = m_blur_buffers[outputId].get();
        horizontal_param->SetAsBool(horizontal);
        image->SetAsTexture(i == 0 ? m_post_buffer.get()
                                   : m_blur_buffers[inputId].get());
        Renderer::DrawNDCQuad(*m_blur_shader);
        horizontal = !horizontal;

        Renderer::EndRenderPass();
    }
}

void PostProcessSystem::RenderPost()
{
    int index = 0;
    RenderSubpassInfo info{&index, 1};
    Renderer::BeginRenderSubpass(info);
    m_post_shader->GetParam("u_bloom")->SetAsBool(m_is_bloom);
    m_post_shader->GetParam("u_bloomFactor")->SetAsFloat(m_bloom_factor);
    m_post_shader->GetParam("u_blur")->SetAsTexture(m_blur_result);

    m_post_shader->GetParam("u_lighting")->SetAsTexture(m_post_buffer.get());
    m_post_shader->GetParam("u_exposure")->SetAsFloat(m_exposure);

    m_post_shader->GetParam("u_gamma")->SetAsFloat(m_gamma_correction);

    Renderer::DrawNDCQuad(*m_post_shader);

    Renderer::EndRenderSubpass();
}

void PostProcessSystem::RenderMipmapTest()
{
    Renderer::BeginRenderPass(
        RenderPassInfo{m_test_target.get(), m_width, m_height});
    m_test_shader->GetParam("u_screen")->SetAsTexture(m_post_buffer.get());
    Renderer::DrawNDCQuad(*m_test_shader);
    Renderer::EndRenderPass();
}

void PostProcessSystem::SetExposure(float exposure) { m_exposure = exposure; }

float PostProcessSystem::GetExposure() { return m_exposure; }

void PostProcessSystem::SetBloom(bool isBloom) { m_is_bloom = isBloom; }

bool PostProcessSystem::GetBloom() { return m_is_bloom; }

void PostProcessSystem::SetBloomFactor(float bloom) { m_bloom_factor = bloom; }

float PostProcessSystem::GetBloomFactor() { return m_bloom_factor; }

void PostProcessSystem::SetGammaCorrection(float gamma)
{
    m_gamma_correction = gamma;
}

float PostProcessSystem::GetGammaCorrection() { return m_gamma_correction; }

}  // namespace SD
