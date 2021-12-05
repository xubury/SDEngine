#include "System/PostProcessSystem.hpp"
#include "Renderer/Renderer.hpp"

namespace SD {

PostProcessSystem::PostProcessSystem(RenderTarget *target, int width,
                                     int height)
    : System("PostProcessSystem"),
      m_blurResult(nullptr),
      m_target(target),
      m_is_bloom(true),
      m_bloom_factor(1.0f),
      m_exposure(1.2),
      m_gamma_correction(1.2) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].AddTexture(
            Texture::Create(width, height, 1, TextureType::TEX_2D,
                            TextureFormat::RGBA, TextureFormatType::FLOAT16));
        m_blurTarget[i].CreateFramebuffer();
    }
    m_postTarget.AddTexture(
        Texture::Create(width, height, 1, TextureType::TEX_2D,
                        TextureFormat::RGBA, TextureFormatType::FLOAT16));
    m_postTarget.CreateFramebuffer();

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
    layout.Push(BufferDataType::FLOAT3);
    layout.Push(BufferDataType::FLOAT2);
    m_quad->AddVertexBuffer(buffer, layout);
    m_quad->SetIndexBuffer(indexBuffer);
}

void PostProcessSystem::OnInit() {
    m_blurShader = ShaderLibrary::Instance().Load("shaders/blur.glsl");
    m_postShader = ShaderLibrary::Instance().Load("shaders/postProcess.glsl");
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
        m_target->GetFramebuffer(), 0, m_postTarget.GetFramebuffer(), 0,
        BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::LINEAR);
    if (m_is_bloom) {
        RenderBlur();
    }
    RenderPost();
    Device::instance().SetDepthMask(true);
}

void PostProcessSystem::OnSizeEvent(const WindowSizeEvent &event) {
    for (int i = 0; i < 2; ++i) {
        m_blurTarget[i].Resize(event.width, event.height);
    }
    m_postTarget.Resize(event.width, event.height);
}

void PostProcessSystem::RenderBlur() {
    const int amount = 10;
    bool horizontal = true;
    m_blurShader->Bind();
    for (int i = 0; i < amount; ++i) {
        const int inputId = horizontal;
        const int outputId = !horizontal;
        renderer->SetRenderTarget(m_blurTarget[outputId]);
        m_blurResult = m_blurTarget[outputId].GetTexture();
        m_blurShader->SetBool("u_horizontal", horizontal);
        m_blurShader->SetTexture("u_image",
                                 i == 0 ? m_postTarget.GetTexture()
                                        : m_blurTarget[inputId].GetTexture());
        renderer->Submit(*m_quad, MeshTopology::TRIANGLES,
                         m_quad->GetIndexBuffer()->GetCount(), 0);
        horizontal = !horizontal;
    }
}

void PostProcessSystem::RenderPost() {
    renderer->SetRenderTarget(*m_target);

    m_postShader->SetBool("u_bloom", m_is_bloom);
    m_postShader->SetFloat("u_bloomFactor", m_bloom_factor);
    m_postShader->SetTexture("u_blur", m_blurResult);

    m_postShader->SetTexture("u_lighting", m_postTarget.GetTexture());
    m_postShader->SetFloat("u_exposure", m_exposure);

    m_postShader->SetFloat("u_gamma", m_gamma_correction);

    m_postShader->Bind();
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
