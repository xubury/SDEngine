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
      m_is_viewport_focused(false),
      m_is_viewport_hovered(false),
      m_hide(false),
      m_load_scene_open(false),
      m_save_scene_open(false) {
    ImGuizmo::SetGizmoSizeClipSpace(0.2);
}

EditorLayer::~EditorLayer() {
    DestroySystem(m_scene_panel);
    DestroySystem(m_editor_camera_system);
    DestroySystem(m_camera_system);
    DestroySystem(m_lighting_system);
    DestroySystem(m_skybox_system);
    DestroySystem(m_sprite_system);
    DestroySystem(m_post_process_system);
    DestroySystem(m_tile_map_system);
    DestroySystem(m_profile_system);
}

void EditorLayer::OnInit() {
    SetViewportSize(0, 0, m_width, m_height);
    // editor related system
    m_scene_panel = CreateSystem<ScenePanel>();
    m_editor_camera_system =
        CreateSystem<EditorCameraSystem>(m_width, m_height);

    // engine logic system
    m_camera_system = CreateSystem<CameraSystem>();
    // normal render systems
    m_lighting_system = CreateSystem<LightingSystem>(m_width, m_height, m_msaa);
    m_skybox_system = CreateSystem<SkyboxSystem>();
    m_sprite_system = CreateSystem<SpriteRenderSystem>();
    m_post_process_system = CreateSystem<PostProcessSystem>(m_width, m_height);
    m_tile_map_system = CreateSystem<TileMapSystem>();
    m_profile_system = CreateSystem<ProfileSystem>(m_width, m_height);

    auto image = asset->LoadAndGet<Bitmap>("icons/light.png");

    m_light_icon = Texture::Create(
        image->Width(), image->Height(),
        TextureSpec(1, TextureType::TEX_2D,
                    image->HasAlpha() ? DataFormat::RGBA : DataFormat::RGB,
                    DataFormatType::UBYTE, TextureWrap::EDGE,
                    TextureMagFilter::LINEAR, TextureMinFilter::LINEAR));
    m_light_icon->SetPixels(0, 0, 0, image->Width(), image->Height(), 1,
                            image->Data());

    PushSystem(m_scene_panel);
    PushSystem(m_editor_camera_system);
    PushSystem(m_camera_system);
    PushSystem(m_skybox_system);
    PushSystem(m_lighting_system);
    PushSystem(m_sprite_system);
    PushSystem(m_post_process_system);
    PushSystem(m_tile_map_system);
    PushSystem(m_profile_system);
}

void EditorLayer::OnPush() {}

void EditorLayer::OnPop() {}

void EditorLayer::OnRender() {
    device->SetFramebuffer(renderer->GetFramebuffer());
    device->Clear();
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
            glm::vec3 pos = transComp.transform.GetWorldPosition();
            float dist = glm::distance(pos, cam->GetWorldPosition());
            float scale = (dist - cam->GetNearZ()) / 20;
            renderer->DrawBillboard(m_light_icon, pos, glm::vec2(scale));
        });

        renderer->End();
        device->Enable(Operation::DEPTH_TEST);
    }
}

void EditorLayer::OnTick(float dt) {
    for (auto &system : GetSystems()) {
        system->OnTick(dt);
    }
    if (m_is_viewport_hovered) {
        glm::vec2 clip = m_viewport.MapScreenToClip(Input::GetMouseCoord());
        Math::Ray ray = scene->GetCamera()->ComputeCameraRay(clip);
        Math::Plane plane(glm::vec3(0, 0, 1), glm::vec3(0));
        glm::vec3 world;
        if (Math::IntersectRayPlane(ray, plane, world)) {
            m_tile_map_system->SelectWorldPos(world);
        }
        if (Input::IsMousePressed(MouseButton::LEFT)) {
            m_tile_map_system->ApplyActionAtPos();
        }
    }
}

void EditorLayer::OnImGui() {
    if (m_hide) return;

    device->BlitFramebuffer(
        renderer->GetFramebuffer(), 0, m_screen_buffer.get(), 0,
        BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::NEAREST);
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        device->BlitFramebuffer(
            m_lighting_system->GetGBuffer(), i, m_debug_gbuffer.get(), i,
            BufferBitMask::COLOR_BUFFER_BIT, BlitFilter::NEAREST);
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
                OpenLoadSceneDialog();
            }

            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
                OpenSaveSceneDialog();
            }

            if (ImGui::MenuItem("Exit", "Esc")) {
                GetApp().Quit();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("GBuffer");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
            ImGui::DrawTexture(*m_debug_gbuffer->GetTexture(i), wsize);
        }
    }
    ImGui::End();
    ImGui::Begin("SSAO");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        ImGui::DrawTexture(*m_lighting_system->GetSSAO(), wsize);
    }
    ImGui::End();

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
        m_is_viewport_focused = ImGui::IsWindowFocused();
        m_is_viewport_hovered = ImGui::IsWindowHovered();
        ImGui::DrawTexture(*m_screen_buffer->GetTexture(), wsize, ImVec2(0, 1),
                           ImVec2(1, 0));
        ImGuizmo::SetRect(m_viewport.GetLeft(), m_viewport.GetTop(),
                          m_viewport.GetWidth(), m_viewport.GetHeight());
        ImGuizmo::SetDrawlist();
        Entity &entity = m_scene_panel->GetSelectedEntity();
        if (entity) {
            Camera *cam = scene->GetCamera();
            ImGuizmo::SetOrthographic(cam->GetCameraType() ==
                                      CameraType::ORTHOGRAPHIC);
            const glm::mat4 &view = cam->GetView();
            const glm::mat4 &projection = cam->GetProjection();

            auto &tc = entity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.transform.GetWorldTransform();
            if (ImGuizmo::Manipulate(
                    glm::value_ptr(view), glm::value_ptr(projection),
                    m_scene_panel->GetGizmoOperation(),
                    m_scene_panel->GetGizmoMode(), glm::value_ptr(transform),
                    nullptr, nullptr)) {
                tc.transform.SetWorldTransform(transform);
            }
        }
        if (!ImGuizmo::IsUsing() && ImGui::IsMouseDown(0) &&
            m_is_viewport_hovered) {
            auto [mouseX, mouseY] = ImGui::GetMousePos();
            mouseX -= m_viewport.GetLeft();
            mouseY = m_viewport.GetHeight() - mouseY + m_viewport.GetTop();
            entt::entity entity_id = Entity::INVALID_ID;
            auto entity_tex =
                m_debug_gbuffer->GetTexture(GeometryBufferType::G_ENTITY_ID);
            // out of bound check
            if (mouseX > 0 && mouseY > 0 && mouseX < entity_tex->GetWidth() &&
                mouseY < entity_tex->GetHeight()) {
                entity_tex->ReadPixels(0, mouseX, mouseY, 0, 1, 1, 1,
                                       sizeof(entity_id), &entity_id);
            }
            if (entity_id != Entity::INVALID_ID) {
                Entity entity(entity_id, scene.get());
                m_scene_panel->SetSelectedEntity(entity);
            }
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();

    ProcessDialog();

    for (auto &system : GetSystems()) {
        system->OnImGui();
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
                GetApp().Quit();
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

void EditorLayer::OnEventsProcess() {}

void EditorLayer::SetViewportSize(uint32_t left, uint32_t top, uint32_t width,
                                  uint32_t height) {
    WindowSizeEvent event;
    event.width = width;
    event.height = height;
    dispatcher->PublishEvent(event);
    m_viewport.SetSize(left, top, width, height);
    renderer->GetDefaultTarget().Resize(width, height);
    m_screen_buffer = Framebuffer::Create();
    m_screen_buffer->AttachTexture(Texture::Create(
        width, height,
        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGB,
                    DataFormatType::UBYTE, TextureWrap::EDGE,
                    TextureMagFilter::NEAREST, TextureMinFilter::NEAREST)));
    m_debug_gbuffer = Framebuffer::Create();
    for (int i = 0; i <= GeometryBufferType::G_ENTITY_ID; ++i) {
        m_debug_gbuffer->AttachTexture(Texture::Create(
            width, height,
            TextureSpec(
                1, TextureType::TEX_2D, GetTextureFormat(GeometryBufferType(i)),
                GetTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
                TextureMagFilter::NEAREST, TextureMinFilter::NEAREST)));
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
    m_file_dialog_info.file_extension = ".scene";
    m_file_dialog_info.directory_path = std::filesystem::current_path();
}

void EditorLayer::OpenSaveSceneDialog() {
    m_save_scene_open = true;
    m_file_dialog_info.type = ImGuiFileDialogType::SAVE_FILE;
    m_file_dialog_info.title = "Save Scene";
    m_file_dialog_info.file_name = "test.scene";
    m_file_dialog_info.file_extension = ".scene";
    m_file_dialog_info.directory_path = std::filesystem::current_path();
}

void EditorLayer::ProcessDialog() {
    if (ImGui::FileDialog(&m_load_scene_open, &m_file_dialog_info)) {
        scene->Load(m_file_dialog_info.result_path.string());
        m_scene_panel->Reset();
    }
    if (ImGui::FileDialog(&m_save_scene_open, &m_file_dialog_info)) {
        scene->Save(m_file_dialog_info.result_path.string());
    }
}

}  // namespace SD
