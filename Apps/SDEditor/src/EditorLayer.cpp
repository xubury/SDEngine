#include "EditorLayer.hpp"
#include "Renderer/Renderer.hpp"
#include "ContentBrowser.hpp"
#include "Core/Input.hpp"
#include "Core/Application.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGuizmo.h"
#include "System/ScriptSystem.hpp"

#include "Loader/TextureLoader.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace SD {

EditorLayer::EditorLayer(int width, int height, int msaa)
    : Layer("EditorLayer"),
      m_viewport_pos(0, 0),
      m_viewport_pos_update(true),
      m_viewport_size(width, height),
      m_viewport_size_update(true),
      m_viewport_focus(false),
      m_viewport_hover(false),
      m_viewport_state_update(true),
      m_msaa(msaa),
      m_is_runtime(false),
      m_quitting(false),
      m_load_scene_open(false),
      m_save_scene_open(false) {
    ImGuizmo::SetGizmoSizeClipSpace(0.2);
}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnInit() {
    Layer::OnInit();

    InitBuffers();
    // editor related system
    m_scene_panel = CreateSystem<ScenePanel>();
    m_editor_camera_system =
        CreateSystem<EditorCameraSystem>(m_viewport_size.x, m_viewport_size.y);

    m_light_icon = TextureLoader::LoadTexture2D("assets/icons/light.png");

    PushSystem(CreateSystem<ContentBrowser>());
    PushSystem(m_scene_panel);
    PushSystem(m_editor_camera_system);
}

void EditorLayer::InitBuffers() {
    m_viewport_buffer = Framebuffer::Create();
    m_viewport_buffer->Attach(TextureSpec(
        m_viewport_size.x, m_viewport_size.y, 1, 1, TextureType::TEX_2D,
        DataFormat::RGB, DataFormatType::UBYTE, TextureWrap::EDGE,
        TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    m_viewport_buffer->Attach(TextureSpec(
        m_viewport_size.x, m_viewport_size.y, 1, 1, TextureType::TEX_2D,
        DataFormat::RED, DataFormatType::UINT, TextureWrap::EDGE,
        TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    m_debug_gbuffer = Framebuffer::Create();
    for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
        m_debug_gbuffer->Attach(TextureSpec(
            m_viewport_size.x, m_viewport_size.y, 1, 1, TextureType::TEX_2D,
            GetTextureFormat(GeometryBufferType(i)),
            GetTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
            TextureMagFilter::NEAREST, TextureMinFilter::NEAREST));
    }

    m_viewport_buffer->Setup();
    m_debug_gbuffer->Setup();
}

void EditorLayer::PushSystems() {
    SD_CORE_ASSERT(m_mode != EditorMode::NONE, "Error editor mode!");
    if (m_mode == EditorMode::THREE_DIMENSIONAL) {
        // engine logic system
        m_camera_system = CreateSystem<CameraSystem>();
        // normal render systems
        m_lighting_system = CreateSystem<LightingSystem>(
            m_viewport_size.x, m_viewport_size.y, m_msaa);
        m_skybox_system = CreateSystem<SkyboxSystem>();
        m_sprite_system = CreateSystem<SpriteRenderSystem>();
        m_post_process_system = CreateSystem<PostProcessSystem>(
            m_viewport_size.x, m_viewport_size.y);
        m_profile_system =
            CreateSystem<ProfileSystem>(m_viewport_size.x, m_viewport_size.y);

        PushSystem(CreateSystem<ScriptSystem>());
        PushSystem(m_camera_system);
        PushSystem(m_skybox_system);
        PushSystem(m_lighting_system);  // lighting is put behind skybox to do
                                        // MSAA with skybox(the background)
        PushSystem(m_sprite_system);    // also sprite is put behind skybox and
                                        // deferred lighting to do blending
        PushSystem(m_post_process_system);
        PushSystem(m_profile_system);

        m_editor_camera_system->AllowRotate(true);
    } else {
        // engine logic system
        m_camera_system = CreateSystem<CameraSystem>();
        // normal render systems
        m_skybox_system = CreateSystem<SkyboxSystem>();
        m_sprite_system = CreateSystem<SpriteRenderSystem>();
        m_post_process_system = CreateSystem<PostProcessSystem>(
            m_viewport_size.x, m_viewport_size.y);
        m_tile_map_system = CreateSystem<TileMapSystem>();
        m_profile_system =
            CreateSystem<ProfileSystem>(m_viewport_size.x, m_viewport_size.y);
        m_animation_editor = CreateSystem<AnimationEditor>();

        PushSystem(CreateSystem<ScriptSystem>());
        PushSystem(m_camera_system);
        PushSystem(m_skybox_system);
        PushSystem(m_sprite_system);  // sprite render need to put behind skybox
                                      // to do blending
        PushSystem(m_post_process_system);
        PushSystem(m_tile_map_system);
        PushSystem(m_animation_editor);
        PushSystem(m_profile_system);

        m_editor_camera_system->AllowRotate(false);
    }
}

void EditorLayer::OnPush() {
    m_entity_select_handler = EventSystem::Get().Register<EntitySelectEvent>(
        [this](const EntitySelectEvent &e) {
            m_selected_entity = {e.entity_id, e.scene};
        });
    m_window_size_handler =
        EventSystem::Get().Register(this, &EditorLayer::OnWindowSizeEvent);
    m_key_handler = EventSystem::Get().Register(this, &EditorLayer::OnKeyEvent);
}

void EditorLayer::OnPop() {
    EventSystem::Get().RemoveHandler(m_entity_select_handler);
    EventSystem::Get().RemoveHandler(m_window_size_handler);
    EventSystem::Get().RemoveHandler(m_key_handler);
}

void EditorLayer::OnRender() {
    if (m_mode == EditorMode::NONE) {
        return;
    }
    Device::Get().SetFramebuffer(renderer->GetFramebuffer());
    Device::Get().Clear();
    uint32_t id = static_cast<uint32_t>(entt::null);
    renderer->GetFramebuffer()->ClearAttachment(1, &id);
    for (auto &system : GetSystems()) {
        system->OnRender();
    }
    if (m_is_runtime) {
        // render to default buffer
        Device::Get().SetFramebuffer(nullptr);
        Device::Get().Clear();
        // Device::Get().BlitToScreen(renderer->GetDefaultTarget());
    } else {
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
    BlitViewportBuffers();
    if (m_mode == EditorMode::THREE_DIMENSIONAL) {
        BlitGeometryBuffers();
    }
}

void EditorLayer::OnTick(float dt) {
    OnViewportUpdate();
    if (m_is_runtime) {
        scene->OnRuntime(dt);
    } else {
        scene->OnEditor(dt, m_editor_camera_system->GetCamera());
    }
    for (auto &system : GetSystems()) {
        system->OnTick(dt);
    }
}

void EditorLayer::OnViewportUpdate() {
    if (m_viewport_size_update) {
        ViewportSizeEvent event{m_viewport_size.x, m_viewport_size.y};
        EventSystem::Get().PublishEvent(event);
        m_viewport_size_update = false;
    }

    if (m_viewport_pos_update) {
        ViewportPosEvent event{m_viewport_pos.x, m_viewport_pos.y};
        EventSystem::Get().PublishEvent(event);
        m_viewport_pos_update = false;
    }

    if (m_viewport_state_update) {
        ViewportStateEvent event{m_viewport_focus, m_viewport_hover};
        EventSystem::Get().PublishEvent(event);
        m_viewport_state_update = false;
    }
}

void EditorLayer::OnImGui() {
    if (m_quitting) {
        ImGui::OpenPopup("Quit?");
    }
    if (ImGui::BeginCenterPopupModal("Quit?")) {
        ImGui::TextUnformatted("Are you sure you want to quit?");
        if (ImGui::Button("Yes")) {
            GetApp().Shutdown();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No")) {
            m_quitting = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (!m_is_runtime) {
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
        DrawDebugBuffers();
        ProcessDialog();
        for (auto &system : GetSystems()) {
            system->OnImGui();
        }
        ImGui::End();
    }
}

void EditorLayer::Hide() {
    m_is_runtime = true;
    SetIsBlockEvent(false);
}

void EditorLayer::Show() {
    m_is_runtime = false;
    SetIsBlockEvent(true);
}

void EditorLayer::Quit() { m_quitting = true; }

void EditorLayer::OnKeyEvent(const KeyEvent &e) {
    if (!e.state) return;

    switch (e.keycode) {
        default:
            break;
        case Keycode::Z: {
            if (IsKeyModActive(e.mod, Keymod::LCTRL)) {
                if (m_is_runtime) {
                    Show();
                } else {
                    Hide();
                }
            }
        } break;
        case Keycode::S: {
            if (IsKeyModActive(e.mod, (Keymod::LCTRL | Keymod::LSHIFT))) {
                OpenSaveSceneDialog();
            } else if (IsKeyModActive(e.mod, Keymod::LSHIFT)) {
                m_scene_panel->SetGizmoOperation(ImGuizmo::SCALE);
            }
        } break;
        case Keycode::N: {
            if (IsKeyModActive(e.mod, Keymod::LCTRL)) {
                NewScene();
            }
        } break;
        case Keycode::L: {
            if (IsKeyModActive(e.mod, Keymod::LCTRL)) {
                OpenLoadSceneDialog();
            }
        } break;
        case Keycode::ESCAPE: {
            Quit();
        } break;
        case Keycode::T: {
            if (IsKeyModActive(e.mod, Keymod::LSHIFT)) {
                m_scene_panel->SetGizmoOperation(ImGuizmo::TRANSLATE);
            }
        } break;
        case Keycode::R: {
            if (IsKeyModActive(e.mod, Keymod::LSHIFT)) {
                m_scene_panel->SetGizmoOperation(ImGuizmo::ROTATE);
            }
        } break;
    }
}

void EditorLayer::OnWindowSizeEvent(const WindowSizeEvent &e) {
    if (m_is_runtime) {
        m_viewport_size.x = e.width;
        m_viewport_size.y = e.height;
    }
}

void EditorLayer::NewScene() {
    scene = CreateRef<Scene>();
    EventSystem::Get().PublishEvent(EntitySelectEvent{entt::null, scene.get()});
}

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
        // auto new_scene =
        //     AssetStorage::Get().LoadAndGet<Scene>(m_file_dialog_info.result_path.string());
        // EventSystem::Get().PublishEvent(NewSceneEvent{new_scene});
        // EventSystem::Get().PublishEvent(EntitySelectEvent{entt::null,
        // scene.get()}); m_scene_panel->Reset();
    }
    if (ImGui::FileDialog(&m_save_scene_open, &m_file_dialog_info)) {
        // AssetStorage::Get().SaveAsset(scene,
        // m_file_dialog_info.result_path.string());
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

void EditorLayer::BlitViewportBuffers() {
    // blit the render color result
    Device::Get().ReadBuffer(renderer->GetFramebuffer(), 0);
    Device::Get().DrawBuffer(m_viewport_buffer.get(), 0);
    Device::Get().BlitFramebuffer(
        renderer->GetFramebuffer(), 0, 0,
        renderer->GetFramebuffer()->GetWidth(),
        renderer->GetFramebuffer()->GetHeight(), m_viewport_buffer.get(), 0, 0,
        m_viewport_buffer->GetWidth(), m_viewport_buffer->GetHeight(),
        BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::NEAREST);
    // blit the entity id result
    Device::Get().ReadBuffer(renderer->GetFramebuffer(), 1);
    Device::Get().DrawBuffer(m_viewport_buffer.get(), 1);
    Device::Get().BlitFramebuffer(
        renderer->GetFramebuffer(), 0, 0,
        renderer->GetFramebuffer()->GetWidth(),
        renderer->GetFramebuffer()->GetHeight(), m_viewport_buffer.get(), 0, 0,
        m_viewport_buffer->GetWidth(), m_viewport_buffer->GetHeight(),
        BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::NEAREST);
}

void EditorLayer::BlitGeometryBuffers() {
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

void EditorLayer::DrawViewport() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Scene");
    {
        const auto &wsize = ImGui::GetContentRegionAvail();
        const auto &min_region = ImGui::GetWindowContentRegionMin();
        const auto &wpos = ImGui::GetWindowPos();
        const glm::ivec2 viewport_size = {wsize.x, wsize.y};
        const glm::ivec2 viewport_pos = {min_region.x + wpos.x,
                                         min_region.y + wpos.y};
        const bool viewport_hover =
            ImGui::IsWindowHovered() && !ImGuizmo::IsOver();
        const bool viewport_focus = ImGui::IsWindowFocused();

        if (m_viewport_size != viewport_size) {
            m_viewport_size = viewport_size;
            m_viewport_buffer->Resize(m_viewport_size.x, m_viewport_size.y);
            m_debug_gbuffer->Resize(m_viewport_size.x, m_viewport_size.y);
            m_viewport_size_update = true;
        }

        if (m_viewport_pos != viewport_pos) {
            m_viewport_pos = viewport_pos;
            m_viewport_pos_update = true;
        }

        if (m_viewport_hover != viewport_hover ||
            m_viewport_focus != viewport_focus) {
            m_viewport_hover = viewport_hover;
            m_viewport_focus = viewport_focus;
            m_viewport_state_update = true;
        }

        const auto &tl_uv = Device::Get().GetUVIndex(0);
        const auto &br_uv = Device::Get().GetUVIndex(2);
        ImGui::DrawTexture(*m_viewport_buffer->GetTexture(),
                           ImVec2(tl_uv.x, tl_uv.y), ImVec2(br_uv.x, br_uv.y));

        ImGuizmo::SetRect(m_viewport_pos.x, m_viewport_pos.y, m_viewport_size.x,
                          m_viewport_size.y);
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
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() &&
            !ImGuizmo::IsUsing()) {
            const auto [mouse_x, mouse_y] = ImGui::GetMousePos();
            const int tex_x = mouse_x - m_viewport_pos.x;
            const int tex_y = m_viewport_size.y - (mouse_y - m_viewport_pos.y);
            entt::entity entity_id = entt::null;
            const Texture *entity_tex = m_viewport_buffer->GetTexture(1);
            // out of bound check
            if (tex_x >= 0 && tex_y >= 0 && tex_x < entity_tex->GetWidth() &&
                tex_y < entity_tex->GetHeight()) {
                entity_tex->ReadPixels(0, tex_x, tex_y, 0, 1, 1, 1,
                                       sizeof(entity_id), &entity_id);
            }
            if (entity_id != entt::null) {
                EventSystem::Get().PublishEvent(
                    EntitySelectEvent{entity_id, scene.get()});
            }
        }
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload =
                    ImGui::AcceptDragDropPayload(DROP_ASSET_ITEM)) {
                std::string filename = static_cast<char *>(payload->Data);
                        SD_TRACE("drop:{}", filename);
                try {
                    Asset *asset = AssetStorage::Get().LoadAsset(filename);
                    if (asset) {
                    }
                } catch (const Exception &e) {
                    SD_ERROR(e.what());
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void EditorLayer::DrawDebugBuffers() {
    if (m_mode == THREE_DIMENSIONAL) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        const auto &tl_uv = Device::Get().GetUVIndex(0);
        const auto &br_uv = Device::Get().GetUVIndex(2);
        ImGui::Begin("GBuffer");
        {
            for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
                ImGui::DrawTexture(*m_debug_gbuffer->GetTexture(i),
                                   ImVec2(tl_uv.x, tl_uv.y),
                                   ImVec2(br_uv.x, br_uv.y));
            }
        }
        ImGui::End();
        ImGui::Begin("SSAO");
        {
            ImGui::DrawTexture(*m_lighting_system->GetSSAO(),
                               ImVec2(tl_uv.x, tl_uv.y),
                               ImVec2(br_uv.x, br_uv.y));
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

}  // namespace SD
