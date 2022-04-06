#include "Core/GraphicsLayer.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Loader/TextureLoader.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "ECS/Component.hpp"

namespace SD {

inline const Vector2f icon_size(5.f);

GraphicsLayer::GraphicsLayer(Device *device, int32_t width, int32_t height,
                             MultiSampleLevel msaa)
    : Layer("Graphics Layer"),
      m_device(device),
      m_width(width),
      m_height(height),
      m_msaa(msaa),
      m_deferred_renderer(width, height, m_msaa),
      m_post_renderer(width, height),
      m_fps(20)
{
}

void GraphicsLayer::InitBuffers()
{
    {
        m_main_target = Framebuffer::Create();
        m_color_buffer =
            Texture::Create(m_width, m_height, 0, m_msaa, TextureType::Normal,
                            DataFormat::RGBA8);
        m_entity_buffer =
            Texture::Create(m_width, m_height, 0, m_msaa, TextureType::Normal,
                            DataFormat::R32UI);
        m_depth_buffer = Renderbuffer::Create(m_width, m_height, m_msaa,
                                              DataFormat::Depth24);
        m_main_target->Attach(*m_color_buffer, 0, 0);
        m_main_target->Attach(*m_entity_buffer, 1, 0);
        m_main_target->Attach(*m_depth_buffer, 0);
    }
}

void GraphicsLayer::OnInit()
{
    Layer::OnInit();
    Renderer::Init(m_device);
    m_light_icon = TextureLoader::LoadTexture2D("assets/icons/light.png");
}

void GraphicsLayer::OnTick(float dt)
{
    // sprite animation
    auto anim_view = m_scene->view<SpriteAnimationComponent>();
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

    m_deferred_renderer.SetRenderSize(width, height);
    m_post_renderer.SetRenderSize(width, height);
}

void GraphicsLayer::SetCamera(Camera *camera) { m_camera = camera; }

void GraphicsLayer::SetRenderScene(Scene *scene) { m_scene = scene; }

void GraphicsLayer::OnRender()
{
    // update camera transform
    auto view = m_scene->view<CameraComponent, TransformComponent>();
    view.each([](CameraComponent &camComp, TransformComponent &trans) {
        camComp.camera.SetWorldTransform(trans.GetWorldTransform().GetMatrix());
    });

    Renderer::BeginRenderPass({m_main_target.get(), m_width, m_height});
    Renderer::SetCamera(*m_camera);
    uint32_t id = static_cast<uint32_t>(entt::null);
    m_main_target->ClearAttachment(1, &id);

    // main render pipeline
    m_skybox_renderer.Render();
    m_deferred_renderer.RenderScene(*m_scene);
    m_sprite_renderer.RenderScene(*m_scene);
    m_post_renderer.Render();

    if (m_debug) {
        const int index[] = {0, 1};
        RenderOperation op;
        op.depth_test = false;
        Renderer::BeginRenderSubpass(RenderSubpassInfo{index, 2, op});

        Renderer2D::Begin();
        auto lightView =
            m_scene->view<DirectionalLightComponent, TransformComponent>();
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

    m_renderer2d_debug_str = Renderer2D::GetDebugInfo();
    m_renderer3d_debug_str = Renderer3D::GetDebugInfo();
    m_fps.Probe();
    Renderer2D::Reset();
    Renderer3D::Reset();
    SD_CORE_ASSERT(Renderer::IsEmptyStack(),
                   "DEBUG: RenderPass Begin/End not pair!")
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
                ImGui::TextWrapped("%s", m_renderer2d_debug_str.c_str());
                ImGui::TextUnformatted("Renderer3D Debug Info:");
                ImGui::TextWrapped("%s", m_renderer3d_debug_str.c_str());
                ImGui::Text("FPS:%.2f(%.2f ms)", m_fps.GetFPS(),
                            m_fps.GetFrameTime());
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Deferred Renderer", flags)) {
                m_deferred_renderer.ImGui();
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx("Post Process Renderer", flags)) {
                m_post_renderer.ImGui();
                ImGui::TreePop();
            }
        }
        ImGui::End();
    }
}

}  // namespace SD
