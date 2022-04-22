#include "Core/GraphicsLayer.hpp"
#include "Core/Application.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "ECS/Component.hpp"
#include "Resource/Resource.hpp"

namespace SD {

inline const Vector2f icon_size(5.f);

GraphicsLayer::GraphicsLayer(ResourceManager *resources, SceneManager *scenes,
                             Device *device, int32_t width, int32_t height,
                             MultiSampleLevel msaa)
    : Layer("GraphicsLayer"),
      m_resources(resources),
      m_scenes(scenes),
      m_device(device),
      m_width(width),
      m_height(height),
      m_msaa(msaa),
      m_color_output(nullptr),
      m_color_output_attachment(0),
      m_fps(20)
{
}

void GraphicsLayer::InitBuffers()
{
    m_color_buffer = Texture::Create(m_width, m_height, 1, m_msaa,
                                     TextureType::Normal2D, DataFormat::RGBA8);
    m_depth_buffer =
        Renderbuffer::Create(m_width, m_height, m_msaa, DataFormat::Depth24);
    m_main_target->Attach(*m_color_buffer, 0, 0);
    m_main_target->Attach(*m_depth_buffer, 0);
}

void GraphicsLayer::OnInit()
{
    m_main_target = Framebuffer::Create();
    InitBuffers();
    Renderer::Init(m_device);
    Renderer2D::Init(m_resources->shaders);
    Renderer3D::Init();
    SkyboxRenderPass::Init(m_resources->shaders,
                           m_resources->textures.Get("skybox/default").Get());
    PostProcessRenderPass::Init(PostProcessSettings{m_width, m_height},
                                m_device, m_resources->shaders);
    DeferredRenderPass::Init(DeferredRenderSettings{m_width, m_height, m_msaa},
                             m_device, m_resources->shaders,
                             m_resources->models);
    SpriteRenderPass::Init(m_resources->textures);
    m_light_icon = m_resources->textures.Get("icon/light");
}

void GraphicsLayer::OnDestroy() {}

void GraphicsLayer::OutputColorBuffer(Framebuffer *framebuffer, int attachment)
{
    m_color_output = framebuffer;
    m_color_output_attachment = attachment;
}

void GraphicsLayer::OnTick(float dt)
{
    // sprite animation

    auto anim_view =
        m_scenes->GetCurrentScene()->view<SpriteAnimationComponent>();
    anim_view.each([&](SpriteAnimationComponent &anim_comp) {
        if (!anim_comp.animations.empty()) {
            anim_comp.animator.Tick(dt);
        }
    });
}

void GraphicsLayer::SetRenderSize(int32_t width, int32_t height)
{
    m_width = width;
    m_height = height;
    InitBuffers();

    DeferredRenderPass::SetRenderSize(width, height);
    PostProcessRenderPass::SetRenderSize(width, height);
}

void GraphicsLayer::SetCamera(Camera *camera) { m_camera = camera; }

void GraphicsLayer::OnRender()
{
    Scene *scene = m_scenes->GetCurrentScene();
    // update camera transform
    auto view = scene->view<CameraComponent, TransformComponent>();
    view.each([](CameraComponent &camComp, TransformComponent &trans) {
        camComp.camera.SetWorldTransform(trans.GetWorldTransform().GetMatrix());
    });

    Renderer::BeginRenderPass({m_main_target.get(), m_width, m_height});
    Renderer::SetCamera(*m_camera);
    uint32_t id = static_cast<uint32_t>(entt::null);
    m_main_target->ClearAttachment(1, &id);

    // main render pipeline
    m_deferred_time = 0;
    m_post_rendering_time = 0;
    Clock clock;

    SkyboxRenderPass::Render();
    clock.Restart();

    DeferredRenderPass::Render(*scene);
    m_deferred_time += clock.Restart();

    SpriteRenderPass::Render(*scene);
    clock.Restart();

    PostProcessRenderPass::Render();
    m_post_rendering_time += clock.Restart();

    if (m_debug) {
        const int index[] = {0, 1};
        RenderOperation op;
        op.depth_test = false;
        Renderer::BeginRenderSubpass(RenderSubpassInfo{index, 2, op});

        Renderer2D::Begin();
        auto lightView =
            scene->view<DirectionalLightComponent, TransformComponent>();
        lightView.each([this](EntityId id, const DirectionalLightComponent &,
                              const TransformComponent &transComp) {
            Vector3f pos = transComp.GetWorldPosition();
            Renderer2D::DrawBillboard(*m_light_icon, pos, icon_size,
                                      glm::vec4(1.0f), static_cast<int>(id));
        });

        Renderer2D::End();
        Renderer::EndRenderSubpass();
    }
    Renderer::EndRenderPass();

    m_fps.Probe();
    SD_CORE_ASSERT(Renderer::IsEmptyStack(),
                   "DEBUG: RenderPass Begin/End not pair!")

    // Blit output
    m_device->ReadBuffer(m_main_target.get(), 0);
    if (m_color_output ||
        (m_color_output == nullptr && m_color_output_attachment == 0)) {
        m_device->DrawBuffer(m_color_output, m_color_output_attachment);
        m_device->BlitFramebuffer(m_main_target.get(), 0, 0, m_width, m_height,
                                  m_color_output, 0, 0, m_width, m_height,
                                  BufferBitMask::ColorBufferBit,
                                  BlitFilter::Nearest);
    }
}

uint32_t GraphicsLayer::ReadEntityId(int x, int y) const
{
    uint32_t id = -1;
    const Texture *entity_buffer = DeferredRenderPass::GetEntityBuffer();
    if (x >= 0 && y >= 0 && x < entity_buffer->GetWidth() &&
        y < entity_buffer->GetHeight()) {
        entity_buffer->ReadPixels(0, x, y, 0, 1, 1, 1, sizeof(id), &id);
    }
    return id;
}

void GraphicsLayer::OnImGui()
{
    if (m_debug) {
        const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Framed |
                                         ImGuiTreeNodeFlags_SpanAvailWidth |
                                         ImGuiTreeNodeFlags_AllowItemOverlap |
                                         ImGuiTreeNodeFlags_FramePadding;
        ImGui::Begin("Graphics Debug");
        {
            if (ImGui::TreeNodeEx("Profiles",
                                  flags | ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::TextUnformatted("Renderer2D Debug Info:");
                ImGui::TextWrapped("%s", Renderer2D::GetDebugInfo().c_str());
                ImGui::TextUnformatted("Renderer3D Debug Info:");
                ImGui::TextWrapped("%s", Renderer3D::GetDebugInfo().c_str());
                ImGui::Text("FPS:%.2f(%.2f ms)", m_fps.GetFPS(),
                            m_fps.GetFrameTime());
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Deferred Render Pass", flags)) {
                ImGui::TextUnformatted("Deferred Rendering Time:");
                ImGui::TextWrapped("%.2f ms", m_deferred_time);
                DeferredRenderPass::ImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Post Process Render Pass", flags)) {
                ImGui::TextUnformatted("Post Process Rendering Time:");
                ImGui::TextWrapped("%.2f ms", m_post_rendering_time);
                PostProcessRenderPass::ImGui();
                ImGui::TreePop();
            }
        }
        ImGui::End();
        // Reset debug info
        Renderer2D::Reset();
        Renderer3D::Reset();
    }
}

}  // namespace SD
