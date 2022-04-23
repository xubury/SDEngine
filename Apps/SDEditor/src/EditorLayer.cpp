#include "EditorLayer.hpp"
#include "EditorEvent.hpp"
#include "Renderer/Renderer.hpp"
#include "ContentBrowser.hpp"
#include "Core/Application.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGuizmo.h"
#include "GridRenderer.hpp"
#include "Utility/String.hpp"
#include "Resource/Resource.hpp"

namespace SD {

EditorLayer::EditorLayer(InputDevice *input, SceneManager *scenes,
                         ResourceManager *resources,
                         GraphicsLayer *graphics_layer, int width, int height)
    : Layer("EditorLayer"),
      m_mode(EditorMode::None),
      m_scenes(scenes),
      m_resources(resources),
      m_graphics_layer(graphics_layer),
      m_scene_panel(&m_dispatcher),
      m_editor_camera(input, width, height),
      m_animation_editor(&m_dispatcher),
      m_content_browser(resources->textures),
      m_viewport_pos(0, 0),
      m_viewport_size(width, height),
      m_is_runtime(false),
      m_quitting(false),
      m_load_scene_open(false),
      m_save_scene_open(false)
{
    ImGuizmo::SetGizmoSizeClipSpace(0.2);
    m_entity_select_handler = m_dispatcher.Register<EntitySelectEvent>(
        [this](const EntitySelectEvent &e) {
            m_selected_entity = {e.entity_id, e.scene};
        });

    GridRenderer::Init();

    m_graphics_layer->SetRenderSize(m_viewport_size.x, m_viewport_size.y);
    m_graphics_layer->SetCamera(&m_editor_camera);

    m_viewport_target = Framebuffer::Create();
    m_graphics_layer->OutputColorBuffer(m_viewport_target.get(), 0);

    InitBuffers();
    m_scenes->EmplaceScene("Empty Scene");
}

EditorLayer::~EditorLayer() {}

void EditorLayer::InitBuffers()
{
    m_scene_buffer = Texture::Create(m_viewport_size.x, m_viewport_size.y, 1,
                                     MultiSampleLevel::None,
                                     TextureType::Normal2D, DataFormat::RGB8);
    m_viewport_target->Attach(*m_scene_buffer, 0, 0);
}

void EditorLayer::OnRender()
{
    RenderPassInfo info;
    info.framebuffer = m_viewport_target.get();
    info.viewport_width = m_viewport_size.x;
    info.viewport_height = m_viewport_size.y;
    info.clear_mask = BufferBitMask::None;
    Renderer::BeginRenderPass(info);
    if (m_mode == EditorMode::TwoDimensional) {
        GridRenderer::Render(m_editor_camera, m_resources->textures,
                             m_tile_map_editor.GetSpriteFrame(),
                             m_tile_map_editor.GetBrush());
    }
    Renderer::EndRenderPass();
}

void EditorLayer::OnTick(float dt)
{
    // disable key event when pop up window show up (prevent bugs)
    if (ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId |
                                        ImGuiPopupFlags_AnyPopupLevel)) {
        return;
    }
    m_editor_camera.Tick(dt);
}

void EditorLayer::OnImGui()
{
    if (m_quitting) {
        ImGui::OpenPopup("Quit?");
    }
    if (ImGui::BeginCenterPopupModal("Quit?")) {
        ImGui::TextUnformatted("Are you sure you want to quit?");
        if (ImGui::Button("Yes")) {
            Application::GetApp().Shutdown();
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
                m_editor_camera.AllowRotate(false);
            }

            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Quit")) {
            ImGui::CloseCurrentPopup();
            Quit();
        }
        ImGui::EndPopup();
    }
    MenuBar();
    DrawViewport();
    ProcessDialog();

    if (m_mode == EditorMode::TwoDimensional) {
        m_tile_map_editor.ImGui(m_resources->textures);
        m_animation_editor.ImGui(m_resources->textures);
    }
    m_scene_panel.ImGui(*m_scenes, m_resources->textures);
    m_editor_camera.ImGui();
    m_content_browser.ImGui();
}

void EditorLayer::Quit() { m_quitting = true; }

void EditorLayer::On(const AppQuitEvent &) { Quit(); }

void EditorLayer::On(const KeyEvent &e)
{
    // disable key event when pop up window show up (prevent bugs)
    if (ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId |
                                        ImGuiPopupFlags_AnyPopupLevel)) {
        return;
    }
    if (!e.state) return;

    switch (e.keycode) {
        default:
            break;
        case Keycode::Z: {
            // TODO: change it to a button
            if (IsKeyModActive(e.mod, Keymod::LCtrl)) {
                // Camera *cam = &m_editor_camera;
                // m_is_runtime = !m_is_runtime;
                // if (m_is_runtime) {
                //     scene->view<CameraComponent>().each(
                //         [&](CameraComponent &cam_comp) {
                //             if (cam_comp.primary) {
                //                 cam = &cam_comp.camera;
                //                 return;
                //             }
                //         });
                // }
                // // change to scene's camera or editor camera
                // m_graphics_layer->SetCamera(cam);
            }
        } break;
        case Keycode::S: {
            if (IsKeyModActive(e.mod, (Keymod::LCtrl | Keymod::LShift))) {
                OpenSaveSceneDialog();
            }
            else if (IsKeyModActive(e.mod, Keymod::LShift)) {
                m_scene_panel.SetGizmoOperation(ImGuizmo::SCALE);
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
                m_scene_panel.SetGizmoOperation(ImGuizmo::TRANSLATE);
            }
        } break;
        case Keycode::R: {
            if (IsKeyModActive(e.mod, Keymod::LShift)) {
                m_scene_panel.SetGizmoOperation(ImGuizmo::ROTATE);
            }
        } break;
    }
}

void EditorLayer::On(const MouseMotionEvent &e)
{
    // disable key event when pop up window show up (prevent bugs)
    if (ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId |
                                        ImGuiPopupFlags_AnyPopupLevel)) {
        return;
    }
    m_editor_camera.Move(e.x_rel, e.y_rel);
}

void EditorLayer::OpenLoadSceneDialog()
{
    m_load_scene_open = true;
    m_file_dialog_info.type = ImGuiFileDialogType::OpenFile;
    m_file_dialog_info.title = "Load Scene";
    m_file_dialog_info.file_name = "";
    m_file_dialog_info.regex_match = SCENE_FILTER;
    m_file_dialog_info.directory_path = "assets";
}

void EditorLayer::OpenSaveSceneDialog()
{
    m_save_scene_open = true;
    m_file_dialog_info.type = ImGuiFileDialogType::SaveFile;
    m_file_dialog_info.title = "Save Scene";
    m_file_dialog_info.file_name = "";
    m_file_dialog_info.regex_match = SCENE_FILTER;
    m_file_dialog_info.directory_path = "assets";
}

void EditorLayer::ProcessDialog()
{
    if (ImGui::FileDialog(&m_load_scene_open, &m_file_dialog_info)) {
    }
    if (ImGui::FileDialog(&m_save_scene_open, &m_file_dialog_info)) {
    }
}

void EditorLayer::MenuBar()
{
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
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

static Entity CreateModelEntity(Scene &scene, const ResourceId rid,
                                const Model &model, const ModelNode *node)
{
    std::string name = node->GetName();
    Entity entity = scene.CreateEntity(name);
    auto &meshes = node->GetMeshes();
    auto &materials = node->GetMaterials();
    for (size_t i = 0; i < meshes.size(); ++i) {
        Entity child = scene.CreateEntity(name + "_" + std::to_string(i));
        entity.AddChild(child);
        auto &mc = child.AddComponent<MeshComponent>();
        mc.model_id = rid;
        mc.mesh_index = meshes[i];
        mc.material = model.GetMaterial(materials[i]);
        auto &tc = child.GetComponent<TransformComponent>();
        tc.SetLocalTransform(node->GetTransform());
    }
    for (auto &child : node->GetChildren()) {
        Entity child_entity = CreateModelEntity(scene, rid, model, child);
        entity.AddChild(child_entity);
    }
    return entity;
}

void EditorLayer::DrawViewport()
{
    Scene *scene = m_scenes->GetCurrentScene();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Scene");
    {
        const auto &wsize = ImGui::GetContentRegionAvail();
        const auto &min_region = ImGui::GetWindowContentRegionMin();
        const auto &wpos = ImGui::GetWindowPos();
        const Vector2i viewport_size = {wsize.x, wsize.y};
        const Vector2i viewport_pos = {min_region.x + wpos.x,
                                       min_region.y + wpos.y};

        if (m_viewport_size != viewport_size && viewport_size.x > 0 &&
            viewport_size.y > 0) {
            m_viewport_size = viewport_size;
            InitBuffers();
            m_editor_camera.Resize(m_viewport_size.x, m_viewport_size.y);
            m_graphics_layer->SetRenderSize(m_viewport_size.x,
                                            m_viewport_size.y);
        }

        if (m_viewport_pos != viewport_pos) {
            m_viewport_pos = viewport_pos;
        }

        if (!m_is_runtime && m_mode == EditorMode::TwoDimensional) {
            if (m_tile_map_editor.ManipulateScene(m_resources->textures,
                                                  m_editor_camera)) {
                Vector3f world = m_tile_map_editor.GetBrush().GetSelectdPos();
                Entity child = scene->CreateEntity("Tile");
                auto &comp = child.AddComponent<SpriteComponent>();
                comp.frame = m_tile_map_editor.GetSpriteFrame();
                child.GetComponent<TransformComponent>().SetWorldPosition(
                    world);
            }
        }

        ImGui::DrawTexture(*m_scene_buffer, ImVec2(0, 1), ImVec2(1, 0));

        // disable ImGuizmo when in runtime
        if (!m_is_runtime) {
            ImGuizmo::SetRect(m_viewport_pos.x, m_viewport_pos.y,
                              m_viewport_size.x, m_viewport_size.y);
            ImGuizmo::SetDrawlist();
            if (m_selected_entity) {
                Camera *cam = &m_editor_camera;
                ImGuizmo::SetOrthographic(cam->GetCameraType() ==
                                          CameraType::Orthographic);
                const Matrix4f &view = cam->GetView();
                const Matrix4f &projection = cam->GetProjection();

                auto &tc = m_selected_entity.GetComponent<TransformComponent>();
                Matrix4f transform = tc.GetWorldTransform().GetMatrix();
                if (ImGuizmo::Manipulate(&view[0][0], &projection[0][0],
                                         m_scene_panel.GetGizmoOperation(),
                                         m_scene_panel.GetGizmoMode(),
                                         &transform[0][0], nullptr, nullptr)) {
                    tc.SetWorldTransform(transform);
                }
            }
        }

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() &&
            !ImGuizmo::IsUsing()) {
            const auto [mouse_x, mouse_y] = ImGui::GetMousePos();
            const int tex_x = mouse_x - m_viewport_pos.x;
            const int tex_y = m_viewport_size.y - (mouse_y - m_viewport_pos.y);
            entt::entity entity_id = static_cast<entt::entity>(
                m_graphics_layer->ReadEntityId(tex_x, tex_y));
            // out of bound check
            if (entity_id != entt::null) {
                m_dispatcher.PublishEvent(EntitySelectEvent{entity_id, scene});
            }
        }
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload =
                    ImGui::AcceptDragDropPayload(DROP_ASSET_ITEM)) {
                try {
                    std::string filename = static_cast<char *>(payload->Data);
                    std::string ext = String::GetExtension(filename);
                    if (ext == ".sdscene") {
                        // SetCurrentScene(
                        //     resource.LoadResource<Scene>(filename).get());
                        // SD_TRACE("load scene asset");
                    }
                    else if (ext == ".obj") {
                        ResourceId rid;
                        auto handle = m_resources->models.Load(rid, filename);
                        CreateModelEntity(*scene, rid, *handle,
                                          handle->GetRootNode());
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
