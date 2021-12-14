#include "System/PostProcessSystem.hpp"
#include "Renderer/Renderer.hpp"

namespace SD {

PostProcessSystem::PostProcessSystem(int width, int height)
    : System("PostProcessSystem"),
      m_blur_target{{0, 0, width, height}, {0, 0, width, height}},
      m_blur_result(nullptr),
      m_post_target(0, 0, width, height),
      m_is_bloom(true),
      m_bloom_factor(1.0f),
      m_exposure(1.2),
      m_gamma_correction(1.2) {
    for (int i = 0; i < 2; ++i) {
        m_blur_target[i].AddTexture(
            TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                        DataFormatType::FLOAT16, TextureWrap::EDGE));
        m_blur_target[i].CreateFramebuffer();
    }
    m_post_target.AddTexture(
        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                    DataFormatType::FLOAT16, TextureWrap::EDGE));
    m_post_target.CreateFramebuffer();

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

void PostProcessSystem::OnRender() {
    Device::instance().SetDepthMask(false);
    Device::instance().BlitFramebuffer(
        renderer->GetFramebuffer(), 0, m_post_target.GetFramebuffer(), 0,
        BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::LINEAR);
    if (m_is_bloom) {
        RenderBlur();
    }
    RenderPost();
    Device::instance().SetDepthMask(true);
}

void PostProcessSystem::OnSizeEvent(const WindowSizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_blur_target[i].Resize(event.width, event.height);
    }
    m_post_target.Resize(event.width, event.height);
}

void PostProcessSystem::RenderBlur() {
    const int amount = 10;
    bool horizontal = true;
    m_blur_shader->Bind();
    for (int i = 0; i < amount; ++i) {
        const int inputId = horizontal;
        const int outputId = !horizontal;
        m_blur_target[outputId].Bind();
        m_blur_result = m_blur_target[outputId].GetTexture();
        m_blur_shader->SetBool("u_horizontal", horizontal);
        m_blur_shader->SetTexture("u_image",
                                  i == 0 ? m_post_target.GetTexture()
                                         : m_blur_target[inputId].GetTexture());
        renderer->Submit(*m_quad, MeshTopology::TRIANGLES,
                         m_quad->GetIndexBuffer()->GetCount(), 0);
        horizontal = !horizontal;
    }
}

void PostProcessSystem::RenderPost() {
    renderer->GetDefaultTarget().Bind();

    m_post_shader->SetBool("u_bloom", m_is_bloom);
    m_post_shader->SetFloat("u_bloomFactor", m_bloom_factor);
    m_post_shader->SetTexture("u_blur", m_blur_result);

    m_post_shader->SetTexture("u_lighting", m_post_target.GetTexture());
    m_post_shader->SetFloat("u_exposure", m_exposure);

    m_post_shader->SetFloat("u_gamma", m_gamma_correction);

    m_post_shader->Bind();
    renderer->Submit(*m_quad, MeshTopology::TRIANGLES,
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
