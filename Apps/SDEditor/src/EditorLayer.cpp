#include "EditorLayer.hpp"
#include "Input/Input.hpp"
#include "Core/Application.hpp"
#include "ECS/Component.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "Asset/Image.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

namespace SD {

EditorLayer::EditorLayer(int width, int height)
    : Layer("Editor Layer"),
      m_width(width),
      m_height(height),
      m_target(0, 0, width, height),
      m_is_viewport_focused(false),
      m_is_viewport_hovered(false),
      m_hide(false),
      m_editor_camera(glm::radians(45.f), width, height, 0.1, 1000.f),
      m_load_scene_open(false),
      m_save_scene_open(false) {
    m_camera_controller.SetCamera(&m_editor_camera);
    m_editor_camera.SetWorldPosition(glm::vec3(0, 0, 10));
    m_screen_buffer = Framebuffer::Create();
    m_screen_buffer->AttachTexture(Texture::Create(
        m_width, m_height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::UBYTE, TextureWrap::BORDER, TextureFilter::NEAREST,
        TextureMipmapFilter::NEAREST));
    m_debug_gbuffer = Framebuffer::Create();
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        m_debug_gbuffer->AttachTexture(Texture::Create(
            m_width, m_height, 1, TextureType::TEX_2D,
            GetTextureFormat(GeometryBufferType(i)),
            GetTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
            TextureFilter::NEAREST, TextureMipmapFilter::NEAREST));
    }
    m_shadow_system = new ShadowSystem();
    m_lighting_system = new LightingSystem(&m_target, m_width, m_height,
                                           GetApp().GetWindow().GetMSAA());
    m_skybox_system = new SkyboxSystem(&m_target);
    m_sprite_system = new SpriteRenderSystem(&m_target);
    m_post_process_system = new PostProcessSystem(&m_target, m_width, m_height);
    m_profile_system = new ProfileSystem(&m_target, m_width, m_height);
}

EditorLayer::~EditorLayer() {
    delete m_shadow_system;
    delete m_lighting_system;
    delete m_skybox_system;
    delete m_sprite_system;
    delete m_post_process_system;
    delete m_profile_system;
}

void EditorLayer::OnPush() {
    NewScene();

    auto resourceId = asset->loadAsset<Image>("icons/light.png");
    auto image = asset->Get<Image>(resourceId);

    m_light_icon = Texture::Create(
        image->width(), image->height(), 1, TextureType::TEX_2D,
        image->hasAlpha() ? TextureFormat::RGBA : TextureFormat::RGB,
        TextureFormatType::UBYTE, TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_LINEAR, image->data());

    m_scene_panel.SetAppVars(MakeAppVars());

    PushSystem(m_shadow_system);
    PushSystem(m_lighting_system);
    PushSystem(m_skybox_system);
    PushSystem(m_sprite_system);
    PushSystem(m_post_process_system);
    PushSystem(m_profile_system);

    Show();
}

void EditorLayer::OnPop() {
    PopSystem(m_shadow_system);
    PopSystem(m_lighting_system);
    PopSystem(m_skybox_system);
    PopSystem(m_sprite_system);
    PopSystem(m_post_process_system);
    PopSystem(m_profile_system);
}

void EditorLayer::OnRender() {
    renderer->SetRenderTarget(m_target);
    renderer->GetDevice().Clear();
    for (auto &system : GetSystems()) {
        system->OnRender();
    }

    if (m_hide) return;

    renderer->BeginScene(m_editor_camera);
    auto lightView = m_scene->view<LightComponent, TransformComponent>();
    lightView.each(
        [this](const LightComponent &, const TransformComponent &transComp) {
            glm::vec3 pos = transComp.transform.GetWorldPosition();
            float dist = glm::distance(pos, m_editor_camera.GetWorldPosition());
            float scale = (dist - m_editor_camera.GetNearZ()) / 20;
            renderer->DrawBillboard(m_light_icon, pos, glm::vec2(scale));
        });
    renderer->EndScene();
}

void EditorLayer::OnTick(float dt) {
    auto [mouseX, mouseY] = ImGui::GetMousePos();
    mouseX -= m_viewport_bounds[0].x;
    mouseY -= m_viewport_bounds[0].y;
    glm::vec2 viewportSize = m_viewport_bounds[1] - m_viewport_bounds[0];
    if (!ImGuizmo::IsUsing() && !m_hide && ImGui::IsMouseDown(0) &&
        m_is_viewport_hovered) {
        entt::entity entity = Entity::INVALID_ID;
        m_debug_gbuffer->ReadPixels(GeometryBufferType::G_ENTITY_ID, 0, mouseX,
                                    viewportSize.y - mouseY, 0, 1, 1, 1,
                                    sizeof(entity), &entity);
        if (entity != Entity::INVALID_ID) {
            m_scene_panel.SetSelectedEntity({entity, m_scene.get()});
        }
    }
    Entity entity = m_scene_panel.GetSelectedEntity();
    if (entity) {
        glm::vec3 pos = entity.GetComponent<TransformComponent>()
                            .transform.GetWorldPosition();
        m_camera_controller.SetFocus(pos);
    }
    m_camera_controller.Tick(dt);
}

void EditorLayer::OnImGui() {
    if (m_hide) {
        return;
    }
    renderer->GetDevice().BlitFramebuffer(
        m_target.GetFramebuffer(), 0, m_screen_buffer.get(), 0,
        BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::NEAREST);
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        renderer->GetDevice().BlitFramebuffer(
            m_lighting_system->GetGBuffer(), i, m_debug_gbuffer.get(), i,
            BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::NEAREST);
    }

    static bool dockspaceOpen = true;
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    static bool fullScreen = true;

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (fullScreen) {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                       ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        windowFlags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
    ImGui::PopStyleVar();

    if (fullScreen) {
        ImGui::PopStyleVar(2);
    }

    ImGuiIO &io = ImGui::GetIO();
    ImGuiStyle &style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
    }

    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                NewScene();
            }

            if (ImGui::MenuItem("Load Scene...", "Ctrl+L")) {
                LoadScene();
            }

            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
                SaveScene();
            }

            if (ImGui::MenuItem("Exit", "Esc")) {
                GetApp().Quit();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    m_scene_panel.OnImGui();

    ImGui::Begin("Render Settings");
    {
        float exposure = m_post_process_system->GetExposure();
        ImGui::TextUnformatted("Exposure");
        if (ImGui::SliderFloat("##Exposure", &exposure, 0, 10)) {
            m_post_process_system->SetExposure(exposure);
        }

        bool isBloom = m_post_process_system->GetBloom();
        if (ImGui::Checkbox("Bloom", &isBloom)) {
            m_post_process_system->SetBloom(isBloom);
        }
        float bloom = m_post_process_system->GetBloomFactor();
        ImGui::TextUnformatted("Bloom Factor");
        if (ImGui::SliderFloat("##Bloom Factor", &bloom, 0.1, 1)) {
            m_post_process_system->SetBloomFactor(bloom);
        }

        float gamma = m_post_process_system->GetGammaCorrection();
        ImGui::TextUnformatted("Gamma Correction");
        if (ImGui::SliderFloat("##Gamma Correction", &gamma, 0.1, 3)) {
            m_post_process_system->SetGammaCorrection(gamma);
        }
    }
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("GBuffer");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
            ImGui::DrawTexture(*m_debug_gbuffer->GetTexture(i), wsize);
        }
    }
    ImGui::End();
    ImGui::Begin("Scene");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_viewport_bounds[0] = {viewportMinRegion.x + viewportOffset.x,
                                viewportMinRegion.y + viewportOffset.y};
        m_viewport_bounds[1] = {viewportMaxRegion.x + viewportOffset.x,
                                viewportMaxRegion.y + viewportOffset.y};
        if (m_width != wsize.x || m_height != wsize.y) {
            if (wsize.x > 0 && wsize.y > 0) {
                m_editor_camera.Resize(wsize.x, wsize.y);
                m_target.Resize(wsize.x, wsize.y);
                m_screen_buffer->Resize(wsize.x, wsize.y);
                m_debug_gbuffer->Resize(wsize.x, wsize.y);
                WindowSizeEvent event;
                event.width = wsize.x;
                event.height = wsize.y;
                dispatcher->publishEvent(event);
            }

            m_width = wsize.x;
            m_height = wsize.y;
        }
        m_is_viewport_focused = ImGui::IsWindowFocused();
        m_is_viewport_hovered = ImGui::IsWindowHovered();
        ImGui::DrawTexture(*m_screen_buffer->GetTexture(), wsize);

        Entity selectedEntity = m_scene_panel.GetSelectedEntity();
        if (selectedEntity) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(m_viewport_bounds[0].x, m_viewport_bounds[0].y,
                              m_viewport_bounds[1].x - m_viewport_bounds[0].x,
                              m_viewport_bounds[1].y - m_viewport_bounds[0].y);
            const glm::mat4 &view = m_editor_camera.GetView();
            const glm::mat4 &projection = m_editor_camera.GetProjection();

            auto &tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.transform.GetWorldTransform();
            if (ImGuizmo::Manipulate(
                    glm::value_ptr(view), glm::value_ptr(projection),
                    m_scene_panel.GetGizmoOperation(),
                    m_scene_panel.GetGizmoMode(), glm::value_ptr(transform),
                    nullptr, nullptr)) {
                tc.transform.SetWorldTransform(transform);
            }
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();

    ProcessDialog();
}

void EditorLayer::Hide() {
    m_hide = true;
    SetIsBlockEvent(false);
}

void EditorLayer::Show() {
    m_hide = false;
    SetIsBlockEvent(true);
    renderer->SetCamera(&m_editor_camera);
}

void EditorLayer::OnEventProcess(const Event &event) {
    if (m_is_viewport_hovered) {
        m_camera_controller.ProcessEvent(event);
    }
    if (event.type == Event::EventType::KEY_PRESSED) {
        switch (event.key.keycode) {
            default:
                break;
            case Keycode::Z: {
                if (event.key.mod == Keymod::LCTRL) {
                    if (m_hide) {
                        Show();
                    } else {
                        Hide();
                    }
                }
            } break;
            case Keycode::S: {
                if (event.key.mod == (Keymod::LCTRL | Keymod::LSHIFT)) {
                    SaveScene();
                } else {
                    m_scene_panel.SetGizmoOperation(ImGuizmo::SCALE);
                }
            } break;
            case Keycode::N: {
                if (event.key.mod == Keymod::LCTRL) {
                    NewScene();
                }
            } break;
            case Keycode::L: {
                if (event.key.mod == Keymod::LCTRL) {
                    LoadScene();
                }
            } break;
            case Keycode::ESCAPE: {
                GetApp().Quit();
            } break;
            case Keycode::T: {
                m_scene_panel.SetGizmoOperation(ImGuizmo::TRANSLATE);
            } break;
            case Keycode::R: {
                m_scene_panel.SetGizmoOperation(ImGuizmo::ROTATE);
            } break;
        }
    }
}

void EditorLayer::OnEventsProcess() {
    if (m_is_viewport_hovered) {
        m_camera_controller.ProcessEvents();
    }
}

void EditorLayer::NewScene() {
    m_scene = CreateRef<Scene>();
    m_scene_panel.SetScene(m_scene.get());
    renderer->SetScene(m_scene.get());
}

void EditorLayer::LoadScene() {
    m_load_scene_open = true;
    m_file_dialog_info.type = ImGuiFileDialogType::OPEN_FILE;
    m_file_dialog_info.title = "Load Scene";
    m_file_dialog_info.file_name = "";
    m_file_dialog_info.file_extension = ".scene";
    m_file_dialog_info.directory_path = std::filesystem::current_path();
}

void EditorLayer::SaveScene() {
    m_save_scene_open = true;
    m_file_dialog_info.type = ImGuiFileDialogType::SAVE_FILE;
    m_file_dialog_info.title = "Save Scene";
    m_file_dialog_info.file_name = "test.scene";
    m_file_dialog_info.file_extension = ".scene";
    m_file_dialog_info.directory_path = std::filesystem::current_path();
}

void EditorLayer::ProcessDialog() {
    if (ImGui::FileDialog(&m_load_scene_open, &m_file_dialog_info)) {
        m_scene->Load(m_file_dialog_info.result_path.string());
    }
    if (ImGui::FileDialog(&m_save_scene_open, &m_file_dialog_info)) {
        m_scene->Save(m_file_dialog_info.result_path.string());
    }
}

}  // namespace SD
