#include "EditorLayer.hpp"
#include "Core/Input.hpp"
#include "Core/Application.hpp"
#include "ECS/Component.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

namespace SD {

EditorLayer::EditorLayer(int width, int height, int msaa)
    : Layer("EditorLayer"),
      m_width(width),
      m_height(height),
      m_msaa(msaa),
      m_hide(false),
      m_quitting(false),
      m_load_scene_open(false),
      m_save_scene_open(false) {
    ImGuizmo::SetGizmoSizeClipSpace(0.2);
}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnInit() {
    SetViewportSize(0, 0, m_width, m_height);
    // editor related system
    m_scene_panel = CreateSystem<ScenePanel>();
    m_editor_camera_system =
        CreateSystem<EditorCameraSystem>(m_width, m_height);

    m_light_icon = asset->LoadAndGet<Sprite>("icons/light.png");

    PushSystem(m_scene_panel);
    PushSystem(m_editor_camera_system);
}

void EditorLayer::PushSystems() {
    SD_CORE_ASSERT(m_mode != EditorMode::NONE, "Error editor mode!");
    if (m_mode == EditorMode::THREE_DIMENSIONAL) {
        // engine logic system
        m_camera_system = CreateSystem<CameraSystem>();
        // normal render systems
        m_lighting_system = CreateSystem<LightingSystem>(m_width, m_height);
        m_skybox_system = CreateSystem<SkyboxSystem>();
        m_sprite_system = CreateSystem<SpriteRenderSystem>();
        m_post_process_system =
            CreateSystem<PostProcessSystem>(m_width, m_height);
        m_profile_system = CreateSystem<ProfileSystem>(m_width, m_height);

        PushSystem(m_camera_system);
        PushSystem(m_lighting_system);
        PushSystem(m_sprite_system);
        PushSystem(m_skybox_system);
        PushSystem(m_post_process_system);
        PushSystem(m_profile_system);

        m_editor_camera_system->AllowRotate(true);
    } else {
        // engine logic system
        m_camera_system = CreateSystem<CameraSystem>();
        // normal render systems
        m_skybox_system = CreateSystem<SkyboxSystem>();
        m_sprite_system = CreateSystem<SpriteRenderSystem>();
        m_post_process_system =
            CreateSystem<PostProcessSystem>(m_width, m_height);
        m_tile_map_system = CreateSystem<TileMapSystem>();
        m_profile_system = CreateSystem<ProfileSystem>(m_width, m_height);

        PushSystem(m_camera_system);
        PushSystem(m_skybox_system);
        PushSystem(m_sprite_system);
        PushSystem(m_post_process_system);
        PushSystem(m_tile_map_system);
        PushSystem(m_profile_system);

        m_editor_camera_system->AllowRotate(false);
    }
}

void EditorLayer::OnPush() {
    m_entity_select_handler = dispatcher->Register<EntitySelectEvent>(
        [this](const EntitySelectEvent &e) {
            m_selected_entity = {e.entity_id, e.scene};
        });
}

void EditorLayer::OnPop() {}

void EditorLayer::OnRender() {
    device->SetFramebuffer(renderer->GetFramebuffer());
    device->Clear();
    renderer->GetFramebuffer()->ClearAttachment(
        1, reinterpret_cast<const int *>(&entt::null));
    for (auto &system : GetSystems()) {
        system->OnRender();
    }
    if (m_hide) {
        renderer->RenderToScreen();
    } else {
        device->Disable(Operation::DEPTH_TEST);

        device->SetTarget(renderer->GetDefaultTarget());
        Camera *cam = scene->GetCamera();
        renderer->Begin(*cam);
        auto lightView = scene->view<LightComponent, TransformComponent>();
        lightView.each([this, &cam](const LightComponent &,
                                    const TransformComponent &transComp) {
            glm::vec3 pos = transComp.GetWorldPosition();
            float dist = glm::distance(pos, cam->GetWorldPosition());
            float scale = (dist - cam->GetNearZ()) / 20;
            renderer->DrawBillboard(m_light_icon->GetTexture(), pos,
                                    glm::vec2(scale));
        });

        renderer->End();
        device->Enable(Operation::DEPTH_TEST);
    }
}

void EditorLayer::OnTick(float dt) {
    for (auto &system : GetSystems()) {
        system->OnTick(dt);
    }
}

void EditorLayer::OnImGui() {
    if (m_quitting) {
        ImGui::OpenPopup("Quit?");
    }
    if (ImGui::BeginCenterPopupModal("Quit?")) {
        ImGui::TextUnformatted("Are you sure you want to quit?");
        if (ImGui::Button("Yes")) {
            GetApp().Quit();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No")) {
            m_quitting = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (!m_hide) {
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
                           ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
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

        if (!m_quitting && m_mode == EditorMode::NONE) {
            ImGui::OpenPopup("Select Editor Mode");
        }
        if (ImGui::BeginCenterPopupModal("Select Editor Mode")) {
            static int mode = EditorMode::TWO_DIMENSIONAL;
            ImGui::TextUnformatted("Please select an editor mode(2D/3D):");
            ImGui::TextUnformatted("Mode:");
            ImGui::SameLine();
            ImGui::RadioButton("2D", &mode, EditorMode::TWO_DIMENSIONAL);
            ImGui::SameLine();
            ImGui::RadioButton("3D", &mode, EditorMode::THREE_DIMENSIONAL);
            if (ImGui::Button("OK")) {
                if (mode != EditorMode::NONE) {
                    m_mode = static_cast<EditorMode>(mode);
                    PushSystems();
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Quit")) {
                Quit();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        MenuBar();
        DrawViewport();
        DebugLighting();
        ProcessDialog();
        for (auto &system : GetSystems()) {
            system->OnImGui();
        }
        ImGui::End();
    }
}

void EditorLayer::Hide() {
    m_hide = true;
    SetIsBlockEvent(false);
    m_editor_camera_system->ActiveEditorCam(false);
    SetViewportSize(0, 0, device->GetWidth(), device->GetHeight());
}

void EditorLayer::Show() {
    m_hide = false;
    SetIsBlockEvent(true);
    m_editor_camera_system->ActiveEditorCam(true);
}

void EditorLayer::Quit() { m_quitting = true; }

void EditorLayer::OnEventProcess(const Event &event) {
    if (event.type == EventType::MOUSE_MOTION) {
        dispatcher->PublishEvent(event.mouse_motion);
    } else if (event.type == EventType::WINDOW_RESIZED) {
        if (m_hide) {
            SetViewportSize(0, 0, event.window_size.width,
                            event.window_size.height);
        }
    } else if (event.type == EventType::KEY) {
        dispatcher->PublishEvent(event.key);

        if (!event.key.state) return;

        switch (event.key.keycode) {
            default:
                break;
            case Keycode::Z: {
                if (IsKeyModActive(event.key.mod, Keymod::LCTRL)) {
                    if (m_hide) {
                        Show();
                    } else {
                        Hide();
                    }
                }
            } break;
            case Keycode::S: {
                if (IsKeyModActive(event.key.mod,
                                   (Keymod::LCTRL | Keymod::LSHIFT))) {
                    OpenSaveSceneDialog();
                } else if (IsKeyModActive(event.key.mod, Keymod::LSHIFT)) {
                    m_scene_panel->SetGizmoOperation(ImGuizmo::SCALE);
                }
            } break;
            case Keycode::N: {
                if (IsKeyModActive(event.key.mod, Keymod::LCTRL)) {
                    NewScene();
                }
            } break;
            case Keycode::L: {
                if (IsKeyModActive(event.key.mod, Keymod::LCTRL)) {
                    OpenLoadSceneDialog();
                }
            } break;
            case Keycode::ESCAPE: {
                Quit();
            } break;
            case Keycode::T: {
                if (IsKeyModActive(event.key.mod, Keymod::LSHIFT)) {
                    m_scene_panel->SetGizmoOperation(ImGuizmo::TRANSLATE);
                }
            } break;
            case Keycode::R: {
                if (IsKeyModActive(event.key.mod, Keymod::LSHIFT)) {
                    m_scene_panel->SetGizmoOperation(ImGuizmo::ROTATE);
                }
            } break;
        }
    }
}

void EditorLayer::SetViewportSize(uint32_t left, uint32_t top, uint32_t width,
                                  uint32_t height) {
    WindowSizeEvent event;
    event.width = width;
    event.height = height;
    dispatcher->PublishEvent(event);
    renderer->GetViewport().SetSize(left, top, width, height);
    renderer->GetDefaultTarget().SetSize(width, height);
    m_screen_buffer = Framebuffer::Create(width, height);
    m_screen_buffer->Attach(
        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGB,
                    DataFormatType::UBYTE, TextureWrap::EDGE,
                    TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    m_screen_buffer->Attach(TextureSpec(1, TextureType::TEX_2D, DataFormat::RED,
                                        DataFormatType::INT, TextureWrap::EDGE,
                                        TextureMagFilter::NEAREST,
                                        TextureMinFilter::NEAREST));
    m_debug_gbuffer = Framebuffer::Create(width, height);
    for (int i = 0; i <= GeometryBufferType::G_ENTITY_ID; ++i) {
        m_debug_gbuffer->Attach(TextureSpec(
            1, TextureType::TEX_2D, GetTextureFormat(GeometryBufferType(i)),
            GetTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
            TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    }

    m_width = width;
    m_height = height;
}

void EditorLayer::NewScene() { scene->clear(); }

void EditorLayer::OpenLoadSceneDialog() {
    m_load_scene_open = true;
    m_file_dialog_info.type = ImGuiFileDialogType::OPEN_FILE;
    m_file_dialog_info.title = "Load Scene";
    m_file_dialog_info.file_name = "";
    m_file_dialog_info.regex_match = SCENE_FILTER;
    m_file_dialog_info.directory_path = std::filesystem::current_path();
}

void EditorLayer::OpenSaveSceneDialog() {
    m_save_scene_open = true;
    m_file_dialog_info.type = ImGuiFileDialogType::SAVE_FILE;
    m_file_dialog_info.title = "Save Scene";
    m_file_dialog_info.file_name = "test.scene";
    m_file_dialog_info.regex_match = SCENE_FILTER;
    m_file_dialog_info.directory_path = std::filesystem::current_path();
}

void EditorLayer::ProcessDialog() {
    if (ImGui::FileDialog(&m_load_scene_open, &m_file_dialog_info)) {
        scene->clear();
        std::ifstream is(m_file_dialog_info.result_path.string(),
                         std::ios::binary);
        cereal::PortableBinaryInputArchive archive(is);
        scene->Deserialize(archive);
        m_scene_panel->Reset();
    }
    if (ImGui::FileDialog(&m_save_scene_open, &m_file_dialog_info)) {
        std::ofstream os(m_file_dialog_info.result_path.string(),
                         std::ios::binary);
        cereal::PortableBinaryOutputArchive archive(os);
        scene->Serialize(archive);
    }
}

void EditorLayer::MenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                NewScene();
            }

            if (ImGui::MenuItem("Load Scene...", "Ctrl+L")) {
                OpenLoadSceneDialog();
            }

            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
                OpenSaveSceneDialog();
            }

            if (ImGui::MenuItem("Exit", "Esc")) {
                Quit();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void EditorLayer::DrawViewport() {
    device->ReadBuffer(renderer->GetFramebuffer(), 0);
    device->DrawBuffer(m_screen_buffer.get(), 0);
    device->BlitFramebuffer(
        renderer->GetFramebuffer(), 0, 0,
        renderer->GetFramebuffer()->GetWidth(),
        renderer->GetFramebuffer()->GetHeight(), m_screen_buffer.get(), 0, 0,
        m_screen_buffer->GetWidth(), m_screen_buffer->GetHeight(),
        BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::NEAREST);
    device->ReadBuffer(renderer->GetFramebuffer(), 1);
    device->DrawBuffer(m_screen_buffer.get(), 1);
    device->BlitFramebuffer(
        renderer->GetFramebuffer(), 0, 0,
        renderer->GetFramebuffer()->GetWidth(),
        renderer->GetFramebuffer()->GetHeight(), m_screen_buffer.get(), 0, 0,
        m_screen_buffer->GetWidth(), m_screen_buffer->GetHeight(),
        BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::NEAREST);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Scene");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportOffset = ImGui::GetWindowPos();
        if (m_width != wsize.x || m_height != wsize.y) {
            if (wsize.x > 0 && wsize.y > 0) {
                SetViewportSize(viewportMinRegion.x + viewportOffset.x,
                                viewportMinRegion.y + viewportOffset.y, wsize.x,
                                wsize.y);
            }
        }
        renderer->GetViewport().SetFocus(ImGui::IsWindowFocused());
        renderer->GetViewport().SetHover(ImGui::IsWindowHovered() &&
                                         !ImGuizmo::IsOver());
        ImGui::DrawTexture(*m_screen_buffer->GetTexture(), wsize, ImVec2(0, 1),
                           ImVec2(1, 0));
        ImGuizmo::SetRect(renderer->GetViewport().GetLeft(),
                          renderer->GetViewport().GetTop(),
                          renderer->GetViewport().GetWidth(),
                          renderer->GetViewport().GetHeight());
        ImGuizmo::SetDrawlist();

        if (m_selected_entity) {
            Camera *cam = scene->GetCamera();
            ImGuizmo::SetOrthographic(cam->GetCameraType() ==
                                      CameraType::ORTHOGRAPHIC);
            const glm::mat4 &view = cam->GetView();
            const glm::mat4 &projection = cam->GetProjection();

            auto &tc = m_selected_entity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.GetWorldTransform().GetMatrix();
            if (ImGuizmo::Manipulate(
                    glm::value_ptr(view), glm::value_ptr(projection),
                    m_scene_panel->GetGizmoOperation(),
                    m_scene_panel->GetGizmoMode(), glm::value_ptr(transform),
                    nullptr, nullptr)) {
                tc.SetWorldTransform(transform);
            }
        }
        if (ImGui::IsMouseDown(0) && renderer->GetViewport().IsHover()) {
            auto [mouseX, mouseY] = ImGui::GetMousePos();
            mouseX -= renderer->GetViewport().GetLeft();
            mouseY = renderer->GetViewport().GetHeight() - mouseY +
                     renderer->GetViewport().GetTop();
            entt::entity entity_id = entt::null;
            auto entity_tex = m_screen_buffer->GetTexture(1);
            // out of bound check
            if (mouseX > 0 && mouseY > 0 && mouseX < entity_tex->GetWidth() &&
                mouseY < entity_tex->GetHeight()) {
                m_debug_gbuffer->ReadPixels(G_ENTITY_ID, 0, mouseX, mouseY, 0,
                                            1, 1, 1, sizeof(entity_id),
                                            &entity_id);
                SD_TRACE("gbuffer id:{}", entity_id);
                entity_tex->ReadPixels(0, mouseX, mouseY, 0, 1, 1, 1,
                                       sizeof(entity_id), &entity_id);
                SD_TRACE("id:{}", entity_id);
            }
            if (entity_id != entt::null) {
                // dispatcher->PublishEvent(
                //     EntitySelectEvent{entity_id, scene.get()});
            }
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorLayer::DebugLighting() {
    if (m_mode == THREE_DIMENSIONAL) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
            device->ReadBuffer(m_lighting_system->GetGBuffer(), i);
            device->DrawBuffer(m_debug_gbuffer.get(), i);
            device->BlitFramebuffer(
                m_lighting_system->GetGBuffer(), 0, 0,
                m_lighting_system->GetGBuffer()->GetWidth(),
                m_lighting_system->GetGBuffer()->GetHeight(),
                m_debug_gbuffer.get(), 0, 0, m_debug_gbuffer->GetWidth(),
                m_debug_gbuffer->GetHeight(), BufferBitMask::COLOR_BUFFER_BIT,
                BlitFilter::NEAREST);
        }
        ImGui::Begin("GBuffer");
        {
            ImVec2 wsize = ImGui::GetContentRegionAvail();
            for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
                ImGui::DrawTexture(*m_debug_gbuffer->GetTexture(i), wsize,
                                   ImVec2(0, 1), ImVec2(1, 0));
            }
        }
        ImGui::End();
        ImGui::Begin("SSAO");
        {
            ImVec2 wsize = ImGui::GetContentRegionAvail();
            ImGui::DrawTexture(*m_lighting_system->GetSSAO(), wsize,
                               ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

}  // namespace SD
