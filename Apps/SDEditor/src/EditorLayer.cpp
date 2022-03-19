#include "EditorLayer.hpp"
#include "Renderer/Renderer.hpp"
#include "ContentBrowser.hpp"
#include "Core/Input.hpp"
#include "Core/Application.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGuizmo.h"

#include <glm/gtc/type_ptr.hpp>

namespace SD {

EditorLayer::EditorLayer(GraphicsLayer *graphics_layer, int width, int height)
    : Layer("EditorLayer"),
      m_mode(EditorMode::None),
      m_graphics_layer(graphics_layer),
      m_viewport_pos(0, 0),
      m_viewport_size(width, height),
      m_viewport_size_update(true),
      m_is_runtime(false),
      m_quitting(false),
      m_load_scene_open(false),
      m_save_scene_open(false)
{
    ImGuizmo::SetGizmoSizeClipSpace(0.2);
}

EditorLayer::~EditorLayer() {}

void EditorLayer::OnInit()
{
    auto &dispatcher = GetEventDispatcher();
    Layer::OnInit();

    InitBuffers();
    // editor related system
    m_scene_panel = CreateSystem<ScenePanel>();
    m_editor_camera_system =
        CreateSystem<EditorCameraSystem>(m_viewport_size.x, m_viewport_size.y);

    PushSystem(CreateSystem<ContentBrowser>());
    PushSystem(m_scene_panel);
    PushSystem(m_editor_camera_system);
    PushSystem(CreateSystem<ProfileSystem>());

    auto &storage = AssetStorage::Get();
    m_scene_asset = storage.CreateAsset<SceneAsset>("default scene");
    dispatcher.PublishEvent(NewSceneEvent{m_scene_asset->GetScene()});
    dispatcher.PublishEvent(CameraEvent{m_editor_camera_system->GetCamera()});
}

void EditorLayer::InitBuffers()
{
    FramebufferCreateInfo info;
    info.width = m_viewport_size.x;
    info.height = m_viewport_size.y;
    info.depth = 0;
    info.attachments.push_back(AttachmentDescription{
        AttachmentType::Normal, DataFormat::RGB8, MultiSampleLevel::X1});
    info.attachments.push_back(AttachmentDescription{
        AttachmentType::Normal, DataFormat::R32UI, MultiSampleLevel::X1});

    m_viewport_buffer = Framebuffer::Create(info);
}

void EditorLayer::OnPush()
{
    auto &dispatcher = GetEventDispatcher();
    m_entity_select_handler = dispatcher.Register<EntitySelectEvent>(
        [this](const EntitySelectEvent &e) {
            m_selected_entity = {e.entity_id, e.scene};
        });
    m_window_size_handler =
        dispatcher.Register(this, &EditorLayer::OnWindowSizeEvent);
    m_key_handler = dispatcher.Register(this, &EditorLayer::OnKeyEvent);
}

void EditorLayer::OnPop()
{
    auto &dispatcher = GetEventDispatcher();
    dispatcher.RemoveHandler(m_entity_select_handler);
    dispatcher.RemoveHandler(m_window_size_handler);
    dispatcher.RemoveHandler(m_key_handler);
}

void EditorLayer::OnRender()
{
    auto *framebuffer = m_graphics_layer->GetFramebuffer();
    RenderPassInfo info;
    info.framebuffer = framebuffer;
    info.viewport_width = framebuffer->GetWidth();
    info.viewport_height = framebuffer->GetHeight();
    info.clear_mask = BufferBitMask::None;
    Renderer::BeginRenderPass(info);
    for (auto &system : GetSystems()) {
        system->OnRender();
    }
    // blit the render color result
    Renderer::BlitToBuffer(0, m_viewport_buffer.get(), 0,
                           BufferBitMask::ColorBufferBit);
    // blit the entity id result
    Renderer::BlitToBuffer(1, m_viewport_buffer.get(), 1,
                           BufferBitMask::ColorBufferBit);
    Renderer::EndRenderPass();
}

void EditorLayer::OnTick(float dt)
{
    OnViewportUpdate();
    if (!m_is_runtime) {
        for (auto &system : GetSystems()) {
            system->OnTick(dt);
        }
    }
}

void EditorLayer::OnViewportUpdate()
{
    if (m_viewport_size_update) {
        auto &dispatcher = GetEventDispatcher();
        RenderSizeEvent event{m_viewport_size.x, m_viewport_size.y};
        dispatcher.PublishEvent(event);
        m_viewport_size_update = false;
    }
}

void EditorLayer::OnImGui()
{
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

    if (!m_quitting && m_mode == EditorMode::None) {
        ImGui::OpenPopup("Select Editor Mode");
    }
    if (ImGui::BeginCenterPopupModal("Select Editor Mode")) {
        static EditorMode mode = EditorMode::TwoDimensional;
        ImGui::TextUnformatted("Please select an editor mode(2D/3D):");
        ImGui::TextUnformatted("Mode:");
        ImGui::SameLine();
        if (ImGui::RadioButton("2D", mode == EditorMode::TwoDimensional)) {
            mode = EditorMode::TwoDimensional;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("3D", mode == EditorMode::ThreeDimensional)) {
            mode = EditorMode::ThreeDimensional;
        }
        if (ImGui::Button("OK")) {
            m_mode = mode;
            if (mode == EditorMode::TwoDimensional) {
                m_tile_map_system = CreateSystem<TileMapSystem>(
                    m_graphics_layer->GetFramebuffer());
                m_animation_editor = CreateSystem<AnimationEditor>();

                PushSystem(m_tile_map_system);
                PushSystem(m_animation_editor);

                m_editor_camera_system->AllowRotate(false);
            }

            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Quit")) {
            Quit();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    for (auto &system : GetSystems()) {
        system->OnImGui();
    }
    MenuBar();
    DrawViewport();
    ProcessDialog();
}

void EditorLayer::Quit() { m_quitting = true; }

void EditorLayer::OnKeyEvent(const KeyEvent &e)
{
    if (!e.state) return;

    switch (e.keycode) {
        default:
            break;
        case Keycode::Z: {
            // TODO: change it to a button
            if (IsKeyModActive(e.mod, Keymod::LCtrl)) {
                Camera *cam = m_editor_camera_system->GetCamera();
                m_is_runtime = !m_is_runtime;
                if (m_is_runtime) {
                    auto view = GetScene().view<CameraComponent>();
                    view.each([&](CameraComponent &cam_comp) {
                        if (cam_comp.primary) {
                            cam = &cam_comp.camera;
                            return;
                        }
                    });
                }
                // change to scene's camera or editor camera
                GetEventDispatcher().PublishEvent(CameraEvent{cam});
            }
        } break;
        case Keycode::S: {
            if (IsKeyModActive(e.mod, (Keymod::LCtrl | Keymod::LShift))) {
                OpenSaveSceneDialog();
            }
            else if (IsKeyModActive(e.mod, Keymod::LShift)) {
                m_scene_panel->SetGizmoOperation(ImGuizmo::SCALE);
            }
        } break;
        case Keycode::N: {
            if (IsKeyModActive(e.mod, Keymod::LCtrl)) {
                NewScene();
            }
        } break;
        case Keycode::L: {
            if (IsKeyModActive(e.mod, Keymod::LCtrl)) {
                OpenLoadSceneDialog();
            }
        } break;
        case Keycode::Escape: {
            Quit();
        } break;
        case Keycode::T: {
            if (IsKeyModActive(e.mod, Keymod::LShift)) {
                m_scene_panel->SetGizmoOperation(ImGuizmo::TRANSLATE);
            }
        } break;
        case Keycode::R: {
            if (IsKeyModActive(e.mod, Keymod::LShift)) {
                m_scene_panel->SetGizmoOperation(ImGuizmo::ROTATE);
            }
        } break;
    }
}

void EditorLayer::OnWindowSizeEvent(const WindowSizeEvent &e)
{
    if (m_is_runtime) {
        m_viewport_size.x = e.width;
        m_viewport_size.y = e.height;
    }
}

void EditorLayer::NewScene()
{
    // scene = CreateRef<Scene>();
    // dispatcher.PublishEvent(EntitySelectEvent{entt::null, scene});
}

void EditorLayer::OpenLoadSceneDialog()
{
    m_load_scene_open = true;
    m_file_dialog_info.type = ImGuiFileDialogType::OpenFile;
    m_file_dialog_info.title = "Load Scene";
    m_file_dialog_info.file_name = "";
    m_file_dialog_info.regex_match = SCENE_FILTER;
    m_file_dialog_info.directory_path = AssetStorage::Get().GetDirectory();
}

void EditorLayer::OpenSaveSceneDialog()
{
    m_save_scene_open = true;
    m_file_dialog_info.type = ImGuiFileDialogType::SaveFile;
    m_file_dialog_info.title = "Save Scene";
    m_file_dialog_info.file_name = m_scene_asset->GetName();
    m_file_dialog_info.regex_match = SCENE_FILTER;
    m_file_dialog_info.directory_path = AssetStorage::Get().GetDirectory();
}

void EditorLayer::ProcessDialog()
{
    if (ImGui::FileDialog(&m_load_scene_open, &m_file_dialog_info)) {
        // AssetStorage::Get().LoadAsset(m_file_dialog_info.result_path.string());
    }
    if (ImGui::FileDialog(&m_save_scene_open, &m_file_dialog_info)) {
        AssetStorage::Get().SaveAsset(m_scene_asset,
                                      m_file_dialog_info.result_path.string());
    }
}

void EditorLayer::MenuBar()
{
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

void EditorLayer::DrawViewport()
{
    auto &dispatcher = GetEventDispatcher();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Scene");
    {
        const auto &wsize = ImGui::GetContentRegionAvail();
        const auto &min_region = ImGui::GetWindowContentRegionMin();
        const auto &wpos = ImGui::GetWindowPos();
        const glm::ivec2 viewport_size = {wsize.x, wsize.y};
        const glm::ivec2 viewport_pos = {min_region.x + wpos.x,
                                         min_region.y + wpos.y};

        if (m_viewport_size != viewport_size && viewport_size.x > 0 &&
            viewport_size.y > 0) {
            m_viewport_size = viewport_size;
            m_viewport_buffer->Resize(m_viewport_size.x, m_viewport_size.y);
            m_viewport_size_update = true;
        }

        if (m_viewport_pos != viewport_pos) {
            m_viewport_pos = viewport_pos;
        }

        if (m_tile_map_system) {
            m_tile_map_system->SetViewport(m_viewport_pos.x, m_viewport_pos.y,
                                           m_viewport_size.x,
                                           m_viewport_size.y);
            m_tile_map_system->ManipulateScene();
        }

        ImGui::DrawTexture(*m_viewport_buffer->GetTexture(), ImVec2(0, 1),
                           ImVec2(1, 0));

        // disable ImGuizmo when in runtime
        if (!m_is_runtime) {
            ImGuizmo::SetRect(m_viewport_pos.x, m_viewport_pos.y,
                              m_viewport_size.x, m_viewport_size.y);
            ImGuizmo::SetDrawlist();
            if (m_selected_entity) {
                Camera *cam = m_editor_camera_system->GetCamera();
                ImGuizmo::SetOrthographic(cam->GetCameraType() ==
                                          CameraType::Orthographic);
                const glm::mat4 &view = cam->GetView();
                const glm::mat4 &projection = cam->GetProjection();

                auto &tc = m_selected_entity.GetComponent<TransformComponent>();
                glm::mat4 transform = tc.GetWorldTransform().GetMatrix();
                if (ImGuizmo::Manipulate(
                        glm::value_ptr(view), glm::value_ptr(projection),
                        m_scene_panel->GetGizmoOperation(),
                        m_scene_panel->GetGizmoMode(),
                        glm::value_ptr(transform), nullptr, nullptr)) {
                    tc.SetWorldTransform(transform);
                }
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
                dispatcher.PublishEvent(
                    EntitySelectEvent{entity_id, &GetScene()});
            }
        }
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload =
                    ImGui::AcceptDragDropPayload(DROP_ASSET_ITEM)) {
                try {
                    std::string filename = static_cast<char *>(payload->Data);
                    Asset *asset = AssetStorage::Get().LoadAsset(filename);
                    if (asset->IsTypeOf<SceneAsset>()) {
                        m_scene_asset = dynamic_cast<SceneAsset *>(asset);
                        NewSceneEvent e{m_scene_asset->GetScene()};
                        dispatcher.PublishEvent(e);
                        SD_TRACE("load scene asset");
                    }
                }
                catch (const Exception &e) {
                    SD_ERROR(e.what());
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

}  // namespace SD
