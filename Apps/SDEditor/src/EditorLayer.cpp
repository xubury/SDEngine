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

EditorLayer::EditorLayer(int width, int height)
    : Layer("EditorLayer"),
      m_width(width),
      m_height(height),
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
    DestroySystem(m_profile_system);
}

void EditorLayer::OnInit() {
    SetViewportBufferSize(m_width, m_height);
    // editor related system
    m_scene_panel = CreateSystem<ScenePanel>();
    m_editor_camera_system =
        CreateSystem<EditorCameraSystem>(m_width, m_height);

    // engine logic system
    m_camera_system = CreateSystem<CameraSystem>();
    // normal render systems
    m_lighting_system =
        CreateSystem<LightingSystem>(m_width, m_height, window->GetMSAA());
    m_skybox_system = CreateSystem<SkyboxSystem>();
    m_sprite_system = CreateSystem<SpriteRenderSystem>();
    m_post_process_system = CreateSystem<PostProcessSystem>(m_width, m_height);
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
    PushSystem(m_profile_system);

    NewScene();
}

void EditorLayer::OnPush() {}

void EditorLayer::OnPop() {}

void EditorLayer::OnRender() {
    renderer->SetRenderTarget(renderer->GetDefaultTarget());
    Device::instance().Clear();
    for (auto &system : GetSystems()) {
        system->OnRender();
    }

    if (m_hide) return;

    Device::instance().Disable(Operation::DEPTH_TEST);
    Camera *cam = renderer->GetCamera();
    renderer->BeginScene(*cam);
    auto lightView = m_scene->view<LightComponent, TransformComponent>();
    lightView.each([this, &cam](const LightComponent &,
                                const TransformComponent &transComp) {
        glm::vec3 pos = transComp.transform.GetWorldPosition();
        float dist = glm::distance(pos, cam->GetWorldPosition());
        float scale = (dist - cam->GetNearZ()) / 20;
        renderer->DrawBillboard(m_light_icon, pos, glm::vec2(scale));
    });
    renderer->EndScene();
    Device::instance().Enable(Operation::DEPTH_TEST);
}

void EditorLayer::OnTick(float dt) {
    for (auto &system : GetSystems()) {
        system->OnTick(dt);
    }
}

void EditorLayer::OnImGui() {
    Device::instance().BlitFramebuffer(
        renderer->GetFramebuffer(), 0, m_screen_buffer.get(), 0,
        BufferBitMask::COLOR_BUFFER_BIT, TextureMagFilter::NEAREST);
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        Device::instance().BlitFramebuffer(
            m_lighting_system->GetGBuffer(), i, m_debug_gbuffer.get(), i,
            BufferBitMask::COLOR_BUFFER_BIT, TextureMagFilter::NEAREST);
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

        bool ssao_state = m_lighting_system->GetSSAOState();
        if (ImGui::Checkbox("SSAO", &ssao_state)) {
            m_lighting_system->SetSSAOState(ssao_state);
        }

        int ssao_power = m_lighting_system->GetSSAOPower();
        ImGui::TextUnformatted("SSAO Power");
        if (ImGui::SliderInt("##SSAO Power", &ssao_power, 1, 32)) {
            m_lighting_system->SetSSAOPower(ssao_power);
        }

        float ssao_radius = m_lighting_system->GetSSAORadius();
        ImGui::TextUnformatted("SSAO Radius");
        if (ImGui::SliderFloat("##SSAO Radius", &ssao_radius, 0.1, 30)) {
            m_lighting_system->SetSSAORadius(ssao_radius);
        }

        float ssao_bias = m_lighting_system->GetSSAOBias();
        ImGui::TextUnformatted("SSAO Bias");
        if (ImGui::SliderFloat("##SSAO Bias", &ssao_bias, 0.01, 2)) {
            m_lighting_system->SetSSAOBias(ssao_bias);
        }
    }
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
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
        ImGui::DrawTexture(*m_lighting_system->GetSSAO(), wsize, ImVec2(0, 1),
                           ImVec2(1, 0));
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
                SetViewportBufferSize(wsize.x, wsize.y);
                WindowSizeEvent event;
                event.width = wsize.x;
                event.height = wsize.y;
                dispatcher->PublishEvent(event);
            }

            m_width = wsize.x;
            m_height = wsize.y;
        }
        m_is_viewport_focused = ImGui::IsWindowFocused();
        m_is_viewport_hovered = ImGui::IsWindowHovered();
        ImGui::DrawTexture(*m_screen_buffer->GetTexture(), wsize, ImVec2(0, 1),
                           ImVec2(1, 0));
        ImGuizmo::SetRect(m_viewport_bounds[0].x, m_viewport_bounds[0].y,
                          m_viewport_bounds[1].x - m_viewport_bounds[0].x,
                          m_viewport_bounds[1].y - m_viewport_bounds[0].y);
        ImGuizmo::SetDrawlist();
        Entity &entity = m_scene_panel->GetSelectedEntity();
        if (entity) {
            Camera *cam = renderer->GetCamera();
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
        auto [mouseX, mouseY] = ImGui::GetMousePos();
        mouseX -= m_viewport_bounds[0].x;
        mouseY -= m_viewport_bounds[0].y;
        glm::vec2 viewport_size = m_viewport_bounds[1] - m_viewport_bounds[0];
        if (!ImGuizmo::IsUsing() && ImGui::IsMouseDown(0) &&
            m_is_viewport_hovered) {
            entt::entity entity = Entity::INVALID_ID;
            int x = mouseX;
            int y = viewport_size.y - mouseY;
            auto entity_tex =
                m_debug_gbuffer->GetTexture(GeometryBufferType::G_ENTITY_ID);
            // out of bound check
            if (x > 0 && y > 0 && x < entity_tex->GetWidth() &&
                y < entity_tex->GetHeight()) {
                entity_tex->ReadPixels(0, x, y, 0, 1, 1, 1, sizeof(entity),
                                       &entity);
            }
            if (entity != Entity::INVALID_ID) {
                m_scene_panel->SetSelectedEntity({entity, m_scene.get()});
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
}

void EditorLayer::Show() {
    m_hide = false;
    SetIsBlockEvent(true);
    m_editor_camera_system->ActiveEditorCam(true);
}

void EditorLayer::OnEventProcess(const Event &event) {
    if (event.type == EventType::MOUSE_MOTION) {
        dispatcher->PublishEvent(event.mouse_motion);
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
                    SaveScene();
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
                    LoadScene();
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

void EditorLayer::SetViewportBufferSize(uint32_t width, uint32_t height) {
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
}

void EditorLayer::NewScene() {
    m_scene = CreateRef<Scene>();
    m_scene_panel->SetScene(m_scene.get());
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
        m_scene_panel->Reset();
    }
    if (ImGui::FileDialog(&m_save_scene_open, &m_file_dialog_info)) {
        m_scene->Save(m_file_dialog_info.result_path.string());
    }
}

}  // namespace SD
