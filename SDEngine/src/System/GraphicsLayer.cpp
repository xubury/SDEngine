#include "System/GraphicsLayer.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Loader/TextureLoader.hpp"

namespace SD {

GraphicsLayer::GraphicsLayer(int32_t width, int32_t height, int8_t msaa)
    : Layer("Graphics Layer"),
      m_mode(GraphicsMode::NONE),
      m_width(width),
      m_height(height),
      m_msaa(msaa) {}

void GraphicsLayer::OnInit() {
    Layer::OnInit();

    m_light_icon = TextureLoader::LoadTexture2D("assets/icons/light.png");
    m_debug_gbuffer = Framebuffer::Create(m_width, m_height, 1, 1);
    for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
        m_debug_gbuffer->Attach(
            AttachmentDescription{AttachmentType::TEXTURE_2D,
                                  GetTextureFormat(GeometryBufferType(i))});
    }
    m_debug_gbuffer->Setup();
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
            renderer->SetSize(e.width, e.height);
        });
}

void GraphicsLayer::OnPop() {
    EventSystem::Get().RemoveHandler(m_viewport_handler);
}

void GraphicsLayer::OnRender() {
    Device::Get().SetFramebuffer(renderer->GetFramebuffer());
    Device::Get().Clear();
    uint32_t id = static_cast<uint32_t>(entt::null);
    renderer->GetFramebuffer()->ClearAttachment(1, &id);
    for (auto &system : GetSystems()) {
        system->OnRender();
    }
    if (m_mode == GraphicsMode::THREE_DIMENSIONAL && m_debug) {
        BlitGeometryBuffers();
    }
    if (m_debug) {
        Device::Get().Disable(Operation::DEPTH_TEST);

        Camera *cam = scene->GetCamera();
        renderer->Begin(renderer->GetFramebuffer(), *cam);
        auto lightView = scene->view<LightComponent, TransformComponent>();
        lightView.each([this, &cam](const LightComponent &,
                                    const TransformComponent &transComp) {
            glm::vec3 pos = transComp.GetWorldPosition();
            float dist = glm::distance(pos, cam->GetWorldPosition());
            float scale = (dist - cam->GetNearZ()) / 20;
            renderer->DrawBillboard(*m_light_icon, pos, glm::vec2(scale));
        });

        renderer->End();
        Device::Get().Enable(Operation::DEPTH_TEST);
    }
}

void GraphicsLayer::InitPipeline() {
    SD_CORE_ASSERT(m_mode != GraphicsMode::NONE, "Invalid graphics mode");
    if (m_mode == GraphicsMode::THREE_DIMENSIONAL) {
        // engine logic system
        m_camera_system = CreateSystem<CameraSystem>();
        // normal render systems
        m_lighting_system =
            CreateSystem<LightingSystem>(m_width, m_height, m_msaa);
        m_skybox_system = CreateSystem<SkyboxSystem>();
        m_sprite_system = CreateSystem<SpriteRenderSystem>();
        m_post_process_system =
            CreateSystem<PostProcessSystem>(m_width, m_height);

        PushSystem(m_camera_system);
        PushSystem(m_skybox_system);
        PushSystem(m_lighting_system);  // lighting is put behind skybox to do
                                        // MSAA with skybox(the background)
        PushSystem(m_sprite_system);    // also sprite is put behind skybox and
                                        // deferred lighting to do blending
        PushSystem(m_post_process_system);

    } else {
        // engine logic system
        m_camera_system = CreateSystem<CameraSystem>();
        // normal render systems
        m_skybox_system = CreateSystem<SkyboxSystem>();
        m_sprite_system = CreateSystem<SpriteRenderSystem>();
        m_post_process_system =
            CreateSystem<PostProcessSystem>(m_width, m_height);

        PushSystem(m_camera_system);
        PushSystem(m_skybox_system);
        PushSystem(m_sprite_system);  // sprite render need to put behind skybox
                                      // to do blending
        PushSystem(m_post_process_system);
    }
}

void GraphicsLayer::OnImGui() {
    if (m_debug) {
        for (auto &system : GetSystems()) {
            system->OnImGui();
        }
        if (m_mode == GraphicsMode::THREE_DIMENSIONAL) {
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
}

void GraphicsLayer::BlitGeometryBuffers() {
    for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
        Device::Get().ReadBuffer(m_lighting_system->GetGBuffer(), i);
        Device::Get().DrawBuffer(m_debug_gbuffer.get(), i);
        Device::Get().BlitFramebuffer(
            m_lighting_system->GetGBuffer(), 0, 0,
            m_lighting_system->GetGBuffer()->GetWidth(),
            m_lighting_system->GetGBuffer()->GetHeight(), m_debug_gbuffer.get(),
            0, 0, m_debug_gbuffer->GetWidth(), m_debug_gbuffer->GetHeight(),
            BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::NEAREST);
    }
}

}  // namespace SD
