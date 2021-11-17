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
      m_isViewportFocused(false),
      m_isViewportHovered(false),
      m_hide(false),
      m_editorCamera(glm::radians(45.f), width, height, 0.1, 1000.f),
      m_loadSceneOpen(false),
      m_saveSceneOpen(false) {
    m_cameraController.SetCamera(&m_editorCamera);
    m_editorCamera.SetWorldPosition(glm::vec3(0, 0, 10));
    m_screenBuffer = Framebuffer::Create();
    m_screenBuffer->AttachTexture(Texture::Create(
        m_width, m_height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::UBYTE, TextureWrap::BORDER, TextureFilter::NEAREST,
        TextureMipmapFilter::NEAREST));
    m_debugGBuffer = Framebuffer::Create();
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        m_debugGBuffer->AttachTexture(Texture::Create(
            m_width, m_height, 1, TextureType::TEX_2D,
            GetTextureFormat(GeometryBufferType(i)),
            GetTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
            TextureFilter::NEAREST, TextureMipmapFilter::NEAREST));
    }
    m_shadowSystem = new ShadowSystem();
    m_lightingSystem = new LightingSystem(&m_target, m_width, m_height,
                                          GetApp().GetWindow().GetSamples());
    m_skyboxSystem = new SkyboxSystem(&m_target);
    m_spriteSystem = new SpriteRenderSystem(&m_target);
    m_postProcessSystem = new PostProcessSystem(&m_target, m_width, m_height);
    m_profileSystem = new ProfileSystem(&m_target, m_width, m_height);
}

EditorLayer::~EditorLayer() {
    delete m_shadowSystem;
    delete m_lightingSystem;
    delete m_skyboxSystem;
    delete m_spriteSystem;
    delete m_postProcessSystem;
    delete m_profileSystem;
}

void EditorLayer::OnPush() {
    NewScene();

    auto resourceId = asset->loadAsset<Image>("icons/light.png");
    auto image = asset->Get<Image>(resourceId);

    m_lightIcon = Texture::Create(
        image->width(), image->height(), 1, TextureType::TEX_2D,
        image->hasAlpha() ? TextureFormat::RGBA : TextureFormat::RGB,
        TextureFormatType::UBYTE, TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_LINEAR, image->data());

    m_scenePanel.SetAppVars(MakeAppVars());

    PushSystem(m_shadowSystem);
    PushSystem(m_lightingSystem);
    PushSystem(m_skyboxSystem);
    PushSystem(m_spriteSystem);
    PushSystem(m_postProcessSystem);
    PushSystem(m_profileSystem);

    Show();
}

void EditorLayer::OnPop() {
    PopSystem(m_shadowSystem);
    PopSystem(m_lightingSystem);
    PopSystem(m_skyboxSystem);
    PopSystem(m_spriteSystem);
    PopSystem(m_postProcessSystem);
    PopSystem(m_profileSystem);
}

void EditorLayer::OnRender() {
    renderer->SetRenderTarget(m_target);
    renderer->GetDevice().Clear();
    for (auto &system : GetSystems()) {
        system->OnRender();
    }

    if (m_hide) return;

    renderer->BeginScene(m_editorCamera);
    auto lightView = m_scene->view<LightComponent, TransformComponent>();
    lightView.each(
        [this](const LightComponent &, const TransformComponent &transComp) {
            glm::vec3 pos = transComp.transform.GetWorldPosition();
            float dist = glm::distance(pos, m_editorCamera.GetWorldPosition());
            float scale = (dist - m_editorCamera.GetNearZ()) / 20;
            renderer->DrawBillboard(m_lightIcon, pos, glm::vec2(scale));
        });
    renderer->EndScene();
}

void EditorLayer::OnTick(float dt) {
    auto [mouseX, mouseY] = ImGui::GetMousePos();
    mouseX -= m_viewportBounds[0].x;
    mouseY -= m_viewportBounds[0].y;
    glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
    if (!ImGuizmo::IsUsing() && !m_hide && ImGui::IsMouseDown(0) &&
        m_isViewportHovered) {
        entt::entity entity = Entity::INVALID_ID;
        m_debugGBuffer->ReadPixels(GeometryBufferType::G_ENTITY_ID, 0, mouseX,
                                   viewportSize.y - mouseY, 0, 1, 1, 1,
                                   sizeof(entity), &entity);
        if (entity != Entity::INVALID_ID) {
            m_scenePanel.SetSelectedEntity({entity, m_scene.get()});
        }
    }
    Entity entity = m_scenePanel.GetSelectedEntity();
    if (entity) {
        glm::vec3 pos = entity.GetComponent<TransformComponent>()
                            .transform.GetWorldPosition();
        m_cameraController.SetFocus(pos);
    }
    m_cameraController.Tick(dt);
}

void EditorLayer::OnImGui() {
    if (m_hide) {
        return;
    }
    renderer->GetDevice().BlitFramebuffer(
        m_target.GetFramebuffer(), 0, m_screenBuffer.get(), 0,
        BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::NEAREST);
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        renderer->GetDevice().BlitFramebuffer(
            m_lightingSystem->GetGBuffer(), i, m_debugGBuffer.get(), i,
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

    m_scenePanel.OnImGui();

    ImGui::Begin("Render Settings");
    {
        float exposure = m_postProcessSystem->GetExposure();
        ImGui::TextUnformatted("Exposure");
        if (ImGui::SliderFloat("##Exposure", &exposure, 0, 10)) {
            m_postProcessSystem->SetExposure(exposure);
        }

        bool isBloom = m_postProcessSystem->GetBloom();
        if (ImGui::Checkbox("Bloom", &isBloom)) {
            m_postProcessSystem->SetBloom(isBloom);
        }
        float bloom = m_postProcessSystem->GetBloomFactor();
        ImGui::TextUnformatted("Bloom Factor");
        if (ImGui::SliderFloat("##Bloom Factor", &bloom, 0.1, 1)) {
            m_postProcessSystem->SetBloomFactor(bloom);
        }

        float gamma = m_postProcessSystem->GetGammaCorrection();
        ImGui::TextUnformatted("Gamma Correction");
        if (ImGui::SliderFloat("##Gamma Correction", &gamma, 0.1, 3)) {
            m_postProcessSystem->SetGammaCorrection(gamma);
        }
    }
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("GBuffer");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
            ImGui::DrawTexture(*m_debugGBuffer->GetTexture(i), wsize);
        }
    }
    ImGui::End();
    ImGui::Begin("Scene");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_viewportBounds[0] = {viewportMinRegion.x + viewportOffset.x,
                               viewportMinRegion.y + viewportOffset.y};
        m_viewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x,
                               viewportMaxRegion.y + viewportOffset.y};
        if (m_width != wsize.x || m_height != wsize.y) {
            if (wsize.x > 0 && wsize.y > 0) {
                m_editorCamera.Resize(wsize.x, wsize.y);
                m_target.Resize(wsize.x, wsize.y);
                m_screenBuffer->Resize(wsize.x, wsize.y);
                m_debugGBuffer->Resize(wsize.x, wsize.y);
                WindowSizeEvent event;
                event.width = wsize.x;
                event.height = wsize.y;
                dispatcher->publishEvent(event);
            }

            m_width = wsize.x;
            m_height = wsize.y;
        }
        m_isViewportFocused = ImGui::IsWindowFocused();
        m_isViewportHovered = ImGui::IsWindowHovered();
        ImGui::DrawTexture(*m_screenBuffer->GetTexture(), wsize);

        Entity selectedEntity = m_scenePanel.GetSelectedEntity();
        if (selectedEntity) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(m_viewportBounds[0].x, m_viewportBounds[0].y,
                              m_viewportBounds[1].x - m_viewportBounds[0].x,
                              m_viewportBounds[1].y - m_viewportBounds[0].y);
            const glm::mat4 &view = m_editorCamera.GetView();
            const glm::mat4 &projection = m_editorCamera.GetProjection();

            auto &tc = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = tc.transform.GetWorldTransform();
            ImGuizmo::OPERATION op =
                static_cast<ImGuizmo::OPERATION>(m_scenePanel.GetGizmoType());
            ImGuizmo::MODE mode =
                static_cast<ImGuizmo::MODE>(m_scenePanel.GetGizmoMode());
            if (ImGuizmo::Manipulate(
                    glm::value_ptr(view), glm::value_ptr(projection), op, mode,
                    glm::value_ptr(transform), nullptr, nullptr)) {
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
    SetBlockEvent(false);
}

void EditorLayer::Show() {
    m_hide = false;
    SetBlockEvent(true);
    renderer->SetCamera(&m_editorCamera);
}

void EditorLayer::OnEventProcess(const Event &event) {
    if (m_isViewportHovered) {
        m_cameraController.ProcessEvent(event);
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
        }
    }
}

void EditorLayer::OnEventsProcess() {
    if (m_isViewportHovered) {
        m_cameraController.ProcessEvents();
    }
}

void EditorLayer::NewScene() {
    m_scene = CreateRef<Scene>();
    m_scenePanel.SetScene(m_scene.get());
    renderer->SetScene(m_scene.get());
}

void EditorLayer::LoadScene() {
    m_loadSceneOpen = true;
    m_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
    m_fileDialogInfo.title = "Load Scene";
    m_fileDialogInfo.fileName = "";
    m_fileDialogInfo.fileExtension = ".scene";
    m_fileDialogInfo.directoryPath = std::filesystem::current_path();
}

void EditorLayer::SaveScene() {
    m_saveSceneOpen = true;
    m_fileDialogInfo.type = ImGuiFileDialogType_SaveFile;
    m_fileDialogInfo.title = "Save Scene";
    m_fileDialogInfo.fileName = "test.scene";
    m_fileDialogInfo.fileExtension = ".scene";
    m_fileDialogInfo.directoryPath = std::filesystem::current_path();
}

void EditorLayer::ProcessDialog() {
    if (ImGui::FileDialog(&m_loadSceneOpen, &m_fileDialogInfo)) {
        m_scene->Load(m_fileDialogInfo.resultPath.string());
    }
    if (ImGui::FileDialog(&m_saveSceneOpen, &m_fileDialogInfo)) {
        m_scene->Save(m_fileDialogInfo.resultPath.string());
    }
}

}  // namespace SD
