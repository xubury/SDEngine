#include "System/GraphicsLayer.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Loader/TextureLoader.hpp"
#include "Renderer/SpriteRenderer.hpp"

namespace SD {

GraphicsLayer::GraphicsLayer(int32_t width, int32_t height,
                             MultiSampleLevel msaa)
    : Layer("Graphics Layer"), m_width(width), m_height(height), m_msaa(msaa) {}

void GraphicsLayer::OnInit() {
    Layer::OnInit();

    {
        FramebufferCreateInfo info;
        info.width = m_width;
        info.height = m_height;
        info.depth = 1;
        info.attachments.push_back(AttachmentDescription{
            AttachmentType::TEXTURE_2D, DataFormat::RGBA8, m_msaa});
        info.attachments.push_back(AttachmentDescription{
            AttachmentType::TEXTURE_2D, DataFormat::R32UI, m_msaa});
        info.attachments.push_back(AttachmentDescription{
            AttachmentType::RENDERBUFFER, DataFormat::DEPTH24, m_msaa});
        m_main_framebuffer = Framebuffer::Create(info);
    }

    {
        FramebufferCreateInfo info;
        m_light_icon = TextureLoader::LoadTexture2D("assets/icons/light.png");
        info.width = m_width;
        info.height = m_height;
        info.depth = 1;
        for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
            info.attachments.push_back(AttachmentDescription{
                AttachmentType::TEXTURE_2D,
                GetTextureFormat(GeometryBufferType(i)), MultiSampleLevel::X1});
        }
        m_debug_gbuffer = Framebuffer::Create(info);
    }

    // engine logic system
    m_camera_system = CreateSystem<CameraSystem>();
    // normal render systems
    m_lighting_system =
        CreateSystem<LightingSystem>(m_main_framebuffer.get(), m_msaa);
    m_skybox_system = CreateSystem<SkyboxSystem>(m_main_framebuffer.get());
    m_sprite_system = CreateSystem<SpriteRenderSystem>();
    m_post_process_system =
        CreateSystem<PostProcessSystem>(m_main_framebuffer.get());

    PushSystem(m_camera_system);
    PushSystem(m_skybox_system);
    PushSystem(m_lighting_system);  // lighting is put behind skybox to do
                                    // MSAA with skybox(the background)
    PushSystem(m_sprite_system);    // also sprite is put behind skybox and
                                    // deferred lighting to do blending
    PushSystem(m_post_process_system);
}

void GraphicsLayer::OnTick(float dt) {
    for (auto &system : GetSystems()) {
        system->OnTick(dt);
    }
}

void GraphicsLayer::OnPush() {
    m_viewport_handler = EventSystem::Get().Register<ViewportSizeEvent>(
        [this](const ViewportSizeEvent &e) {
            m_width = e.width;
            m_height = e.height;
            m_debug_gbuffer->Resize(e.width, e.height);
            m_main_framebuffer->Resize(e.width, e.height);
        });
}

void GraphicsLayer::OnPop() {
    EventSystem::Get().RemoveHandler(m_viewport_handler);
}

void GraphicsLayer::OnRender() {
    Renderer::BeginRenderPass({m_main_framebuffer.get()});
    device->SetFramebuffer(m_main_framebuffer.get());
    device->Clear();
    uint32_t id = static_cast<uint32_t>(entt::null);
    m_main_framebuffer->ClearAttachment(1, &id);
    for (auto &system : GetSystems()) {
        system->OnRender();
    }
    if (m_debug) {
        BlitGeometryBuffers();
    }
    if (m_debug) {
        const int index = 0;
        Renderer::BeginRenderSubpass(RenderSubpassInfo{&index, 1});
        device->Disable(Operation::DEPTH_TEST);

        Camera *cam = scene->GetCamera();
        SpriteRenderer::Begin(*cam);
        auto lightView = scene->view<LightComponent, TransformComponent>();
        lightView.each([this, &cam](const LightComponent &,
                                    const TransformComponent &transComp) {
            glm::vec3 pos = transComp.GetWorldPosition();
            float dist = glm::distance(pos, cam->GetWorldPosition());
            float scale = (dist - cam->GetNearZ()) / 20;
            SpriteRenderer::DrawBillboard(*m_light_icon, pos, glm::vec2(scale));
        });

        SpriteRenderer::End();
        device->Enable(Operation::DEPTH_TEST);
        Renderer::EndRenderSubpass();
    }
    Renderer::EndRenderPass();
}

void GraphicsLayer::OnImGui() {
    if (m_debug) {
        for (auto &system : GetSystems()) {
            system->OnImGui();
        }
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("GBuffer");
        {
            for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
                ImGui::DrawTexture(*m_debug_gbuffer->GetTexture(i),
                                   ImVec2(0, 1), ImVec2(1, 0));
            }
        }
        ImGui::End();
        ImGui::Begin("SSAO");
        {
            ImGui::DrawTexture(*m_lighting_system->GetSSAO(), ImVec2(0, 1),
                               ImVec2(1, 0));
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

void GraphicsLayer::BlitGeometryBuffers() {
    for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
        device->ReadBuffer(m_lighting_system->GetGBuffer(), i);
        device->DrawBuffer(m_debug_gbuffer.get(), i);
        device->BlitFramebuffer(
            m_lighting_system->GetGBuffer(), 0, 0,
            m_lighting_system->GetGBuffer()->GetWidth(),
            m_lighting_system->GetGBuffer()->GetHeight(), m_debug_gbuffer.get(),
            0, 0, m_debug_gbuffer->GetWidth(), m_debug_gbuffer->GetHeight(),
            BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::NEAREST);
    }
}

}  // namespace SD
