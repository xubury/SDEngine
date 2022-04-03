#include "Renderer/GraphicsLayer.hpp"
#include "Renderer/Event.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Loader/TextureLoader.hpp"
#include "Renderer/Renderer2D.hpp"
#include "ECS/Component.hpp"

namespace SD {

inline const Vector2f icon_size(5.f);

GraphicsLayer::GraphicsLayer(Device *device, int32_t width, int32_t height,
                             MultiSampleLevel msaa)
    : Layer("Graphics Layer"),
      m_device(device),
      m_width(width),
      m_height(height),
      m_msaa(msaa)
{
    CreateDispatcher();
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

    {
        m_geometry_target = Framebuffer::Create();
        for (size_t i = 0; i < m_geometry_buffers.size(); ++i) {
            m_geometry_buffers[i] = Texture::Create(
                m_width, m_height, 0, MultiSampleLevel::None,
                TextureType::Normal, GetTextureFormat(GeometryBufferType(i)));
            m_geometry_target->Attach(*m_geometry_buffers[i], i, 0);
        }
    }
}

void GraphicsLayer::OnInit()
{
    Layer::OnInit();
    Renderer::Init(m_device);
    m_light_icon = TextureLoader::LoadTexture2D("assets/icons/light.png");

    // engine logic system
    m_camera_system = CreateSystem<CameraSystem>();
    // normal render systems
    m_lighting_system = CreateSystem<LightingSystem>(m_width, m_height, m_msaa);
    m_skybox_system = CreateSystem<SkyboxSystem>();
    m_sprite_system = CreateSystem<SpriteRenderSystem>();
    m_post_process_system = CreateSystem<PostProcessSystem>(m_width, m_height);

    PushSystem(m_camera_system);
    PushSystem(m_skybox_system);
    PushSystem(m_lighting_system);  // lighting is put behind skybox to do
                                    // MSAA with skybox(the background)
    PushSystem(m_sprite_system);    // also sprite is put behind skybox and
                                    // deferred lighting to do blending
    PushSystem(m_post_process_system);
}

void GraphicsLayer::OnTick(float dt)
{
    for (auto &system : GetSystems()) {
        system->OnTick(dt);
    }
}

void GraphicsLayer::SetRenderSize(int32_t width, int32_t height)
{
    m_width = width;
    m_height = height;
    InitBuffers();
    GetEventDispatcher().PublishEvent(RenderSizeEvent{m_width, m_height});
}

void GraphicsLayer::SetCamera(Camera *camera)
{
    m_camera = camera;
    GetEventDispatcher().PublishEvent(CameraEvent{m_camera});
}

void GraphicsLayer::SetRenderScene(Scene *scene)
{
    GetEventDispatcher().PublishEvent(NewSceneEvent{scene});
}

void GraphicsLayer::OnRender()
{
    Renderer::BeginRenderPass({m_main_target.get(), m_width, m_height});
    uint32_t id = static_cast<uint32_t>(entt::null);
    m_main_target->ClearAttachment(1, &id);
    for (auto &system : GetSystems()) {
        system->OnRender();
    }
    if (m_debug) {
        const int index[] = {0, 1};
        RenderOperation op;
        op.depth_test = false;
        Renderer::BeginRenderSubpass(RenderSubpassInfo{index, 2, op});

        Renderer2D::Begin(*m_camera);
        auto lightView = GetScene().view<LightComponent, TransformComponent>();
        lightView.each([this](EntityId id, const LightComponent &,
                              const TransformComponent &transComp) {
            Vector3f pos = transComp.GetWorldPosition();
            Renderer2D::DrawBillboard(*m_light_icon, pos, icon_size,
                                      glm::vec4(1.0f), static_cast<int>(id));
        });

        Renderer2D::End();
        Renderer::EndRenderSubpass();
    }
    Renderer::EndRenderPass();

    if (m_debug) {
        BlitGeometryBuffers();
    }
    SD_CORE_ASSERT(Renderer::IsEmptyStack(),
                   "DEBUG: RenderPass Begin/End not pair!")
}

void GraphicsLayer::OnImGui()
{
    if (m_debug) {
        for (auto &system : GetSystems()) {
            system->OnImGui();
        }
        ImGui::Begin("GBuffer");
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        {
            for (size_t i = 0; i < m_geometry_buffers.size(); ++i) {
                ImGui::DrawTexture(*m_geometry_buffers[i], ImVec2(0, 1),
                                   ImVec2(1, 0));
            }
        }
        ImGui::PopStyleVar();
        ImGui::End();
    }
}

void GraphicsLayer::BlitGeometryBuffers()
{
    Renderer::BeginRenderPass({m_geometry_target.get(), m_width, m_height});
    for (int i = 0; i < static_cast<int>(GeometryBufferType::EntityId); ++i) {
        Renderer::BlitFromBuffer(i, m_lighting_system->GetGBuffer(), i,
                                 BufferBitMask::ColorBufferBit);
    }
    Renderer::EndRenderPass();
}

}  // namespace SD
