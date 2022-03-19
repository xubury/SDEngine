#include "System/PostProcessSystem.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Renderer/Renderer.hpp"

#include "Loader/ShaderLoader.hpp"

namespace SD {

PostProcessSystem::PostProcessSystem(int32_t width, int32_t height)
    : System("PostProcessSystem"),
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
    System::OnInit();
    m_blur_shader = ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                             "assets/shaders/blur.frag.glsl");
    m_post_shader =
        ShaderLoader::LoadShader("assets/shaders/quad.vert.glsl",
                                 "assets/shaders/post_process.frag.glsl");
    InitBuffers();
}

void PostProcessSystem::InitBuffers()
{
    AttachmentDescription attach_desc{
        AttachmentType::Normal, DataFormat::RGBA16F, MultiSampleLevel::X1};
    for (int i = 0; i < 2; ++i) {
        m_blur_buffer[i] =
            Framebuffer::Create({m_width, m_height, 0, {attach_desc}});
    }
    m_post_buffer = Framebuffer::Create({m_width, m_height, 0, {attach_desc}});
}

void PostProcessSystem::OnPush()
{
    m_is_bloom = setting->GetBoolean("post process", "bloom", m_is_bloom);
    m_bloom_factor =
        setting->GetFloat("post process", "bloom factor", m_bloom_factor);
    m_exposure = setting->GetFloat("post process", "exposure", m_exposure);
    m_gamma_correction = setting->GetFloat("post process", "gamma correction",
                                           m_gamma_correction);
    m_size_handler =
        EventSystem::Get().Register(this, &PostProcessSystem::OnSizeEvent);
}

void PostProcessSystem::OnPop()
{
    setting->SetBoolean("post process", "bloom", m_is_bloom);
    setting->SetFloat("post process", "bloom factor", m_bloom_factor);
    setting->SetFloat("post process", "exposure", m_exposure);
    setting->SetFloat("post process", "gamma correction", m_gamma_correction);
    EventSystem::Get().RemoveHandler(m_size_handler);
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
}

void PostProcessSystem::OnRender()
{
    Renderer::BlitToBuffer(0, m_post_buffer.get(), 0,
                           BufferBitMask::ColorBufferBit);
    if (m_is_bloom) {
        RenderBlur();
    }
    RenderPost();
}

void PostProcessSystem::OnSizeEvent(const ViewportSizeEvent &event)
{
    m_width = event.width;
    m_height = event.height;
    for (auto &buffer : m_blur_buffer) {
        buffer->Resize(event.width, event.height);
    }
    m_post_buffer->Resize(event.width, event.height);
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
        Renderer::BeginRenderPass(RenderPassInfo{
            m_blur_buffer[outputId].get(), m_blur_buffer[outputId]->GetWidth(),
            m_blur_buffer[outputId]->GetHeight()});
        m_blur_result = m_blur_buffer[outputId]->GetTexture();
        horizontal_param->SetAsBool(horizontal);
        image->SetAsTexture(i == 0 ? m_post_buffer->GetTexture()
                                   : m_blur_buffer[inputId]->GetTexture());
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

    m_post_shader->GetParam("u_lighting")
        ->SetAsTexture(m_post_buffer->GetTexture());
    m_post_shader->GetParam("u_exposure")->SetAsFloat(m_exposure);

    m_post_shader->GetParam("u_gamma")->SetAsFloat(m_gamma_correction);

    Renderer::DrawNDCQuad(*m_post_shader);

    Renderer::EndRenderSubpass();
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
