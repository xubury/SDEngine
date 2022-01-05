#include "System/PostProcessSystem.hpp"
#include "ImGui/ImGuiWidget.hpp"

namespace SD {

PostProcessSystem::PostProcessSystem(int width, int height)
    : System("PostProcessSystem"),
      m_width(width),
      m_height(height),
      m_blur_result(nullptr),
      m_is_bloom(true),
      m_bloom_factor(1.0f),
      m_exposure(1.2),
      m_gamma_correction(1.2) {
    const float quadVertices[] = {
        -1.0f, -1.0f, 0.f, 0.f,  0.f,   // bottom left
        1.0f,  -1.0f, 0.f, 1.0f, 0.f,   // bottom right
        1.0f,  1.0f,  0.f, 1.0f, 1.0f,  // top right
        -1.0f, 1.0f,  0.f, 0.f,  1.0f,  // top left
    };
    const uint32_t indices[] = {0, 1, 2, 2, 3, 0};
    auto buffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices),
                                       BufferIOType::STATIC);
    auto indexBuffer = IndexBuffer::Create(indices, 6, BufferIOType::STATIC);
    m_quad = VertexArray::Create();
    VertexBufferLayout layout;
    layout.Push(BufferLayoutType::FLOAT3);
    layout.Push(BufferLayoutType::FLOAT2);
    m_quad->AddVertexBuffer(buffer, layout);
    m_quad->SetIndexBuffer(indexBuffer);
}

void PostProcessSystem::OnInit() {
    m_blur_shader = ShaderLibrary::Instance().Load("shaders/blur.glsl");
    m_post_shader = ShaderLibrary::Instance().Load("shaders/post_process.glsl");
    InitBuffers();
}

void PostProcessSystem::InitBuffers() {
    for (int i = 0; i < 2; ++i) {
        m_blur_buffer[i] = Framebuffer::Create(m_width, m_height);
        m_blur_buffer[i]->Attach(
            TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                        DataFormatType::FLOAT16, TextureWrap::EDGE));
    }
    m_post_buffer = Framebuffer::Create(m_width, m_height);
    m_post_buffer->Attach(TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                      DataFormatType::FLOAT16,
                                      TextureWrap::EDGE));
}

void PostProcessSystem::OnPush() {
    m_is_bloom = ini->GetBoolean("post process", "bloom", m_is_bloom);
    m_bloom_factor =
        ini->GetFloat("post process", "bloom factor", m_bloom_factor);
    m_exposure = ini->GetFloat("post process", "exposure", m_exposure);
    m_gamma_correction =
        ini->GetFloat("post process", "gamma correction", m_gamma_correction);
    m_size_handler =
        dispatcher->Register(this, &PostProcessSystem::OnSizeEvent);
}

void PostProcessSystem::OnPop() {
    ini->SetBoolean("post process", "bloom", m_is_bloom);
    ini->SetFloat("post process", "bloom factor", m_bloom_factor);
    ini->SetFloat("post process", "exposure", m_exposure);
    ini->SetFloat("post process", "gamma correction", m_gamma_correction);
    dispatcher->RemoveHandler(m_size_handler);
}

void PostProcessSystem::OnImGui() {
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

void PostProcessSystem::OnRender() {
    device->SetDepthMask(false);
    device->ReadBuffer(renderer->GetFramebuffer(), 0);
    device->DrawBuffer(m_post_buffer.get(), 0);
    device->BlitFramebuffer(
        renderer->GetFramebuffer(), 0, 0,
        renderer->GetFramebuffer()->GetWidth(),
        renderer->GetFramebuffer()->GetHeight(), m_post_buffer.get(), 0, 0,
        m_post_buffer->GetWidth(), m_post_buffer->GetHeight(),
        BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::NEAREST);
    if (m_is_bloom) {
        RenderBlur();
    }
    RenderPost();
    device->SetDepthMask(true);
}

void PostProcessSystem::OnSizeEvent(const WindowSizeEvent &event) {
    m_width = event.width;
    m_height = event.height;
    InitBuffers();
}

void PostProcessSystem::RenderBlur() {
    const int amount = 10;
    bool horizontal = true;
    for (int i = 0; i < amount; ++i) {
        const int inputId = horizontal;
        const int outputId = !horizontal;
        device->SetFramebuffer(m_blur_buffer[outputId].get());
        m_blur_result = m_blur_buffer[outputId]->GetTexture();
        m_blur_shader->SetBool("u_horizontal", horizontal);
        m_blur_shader->SetTexture(
            "u_image", i == 0 ? m_post_buffer->GetTexture()
                              : m_blur_buffer[inputId]->GetTexture());
        renderer->Submit(*m_blur_shader, *m_quad, MeshTopology::TRIANGLES,
                         m_quad->GetIndexBuffer()->GetCount(), 0);
        horizontal = !horizontal;
    }
}

void PostProcessSystem::RenderPost() {
    device->SetFramebuffer(renderer->GetFramebuffer());
    device->DrawBuffer(renderer->GetFramebuffer(), 0);  // only draw colors
    m_post_shader->SetBool("u_bloom", m_is_bloom);
    m_post_shader->SetFloat("u_bloomFactor", m_bloom_factor);
    m_post_shader->SetTexture("u_blur", m_blur_result);

    m_post_shader->SetTexture("u_lighting", m_post_buffer->GetTexture());
    m_post_shader->SetFloat("u_exposure", m_exposure);

    m_post_shader->SetFloat("u_gamma", m_gamma_correction);

    renderer->Submit(*m_post_shader.get(), *m_quad, MeshTopology::TRIANGLES,
                     m_quad->GetIndexBuffer()->GetCount(), 0);
}

void PostProcessSystem::SetExposure(float exposure) { m_exposure = exposure; }

float PostProcessSystem::GetExposure() { return m_exposure; }

void PostProcessSystem::SetBloom(bool isBloom) { m_is_bloom = isBloom; }

bool PostProcessSystem::GetBloom() { return m_is_bloom; }

void PostProcessSystem::SetBloomFactor(float bloom) { m_bloom_factor = bloom; }

float PostProcessSystem::GetBloomFactor() { return m_bloom_factor; }

void PostProcessSystem::SetGammaCorrection(float gamma) {
    m_gamma_correction = gamma;
}

float PostProcessSystem::GetGammaCorrection() { return m_gamma_correction; }

}  // namespace SD
