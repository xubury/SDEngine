#include "EditorLayer.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Renderer/Renderer.hpp"
#include "Core/Application.hpp"
#include "Core/Input.hpp"
#include "ECS/Component.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"

EditorLayer::EditorLayer(int width, int height)
    : sd::Layer("Editor Layer"),
      m_width(width),
      m_height(height),
      m_isViewportFocused(false),
      m_isViewportHovered(false),
      m_hide(false),
      m_editorCamera(45.f, static_cast<float>(width) / height, 0.1, 100000.f),
      m_loadSceneOpen(false),
      m_saveSceneOpen(false) {
    m_cameraController.setCamera(&m_editorCamera);
    m_editorCamera.setWorldPosition(glm::vec3(0, 0, 10));
    newScene();
}

void EditorLayer::onAttach() {
    sd::Ref<sd::Texture> multisampleTexture = sd::Texture::create(
        m_width, m_height, 8, sd::TextureType::TEX_2D_MULTISAMPLE,
        sd::TextureFormat::RGBA, sd::TextureFormatType::FLOAT,
        sd::TextureWrap::BORDER, sd::TextureFilter::LINEAR,
        sd::TextureMipmapFilter::LINEAR_NEAREST);
    m_target.addTexture(multisampleTexture);
    m_target.addTexture(sd::Texture::create(
        multisampleTexture->getWidth(), multisampleTexture->getHeight(), 8,
        sd::TextureType::TEX_2D_MULTISAMPLE, sd::TextureFormat::ALPHA,
        sd::TextureFormatType::UINT, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST));
    m_target.addTexture(sd::Texture::create(
        multisampleTexture->getWidth(), multisampleTexture->getHeight(), 8,
        sd::TextureType::TEX_2D_MULTISAMPLE, sd::TextureFormat::DEPTH,
        sd::TextureFormatType::FLOAT, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST));
    m_target.init();

    m_framebuffer = sd::Framebuffer::create();
    m_framebuffer->attachTexture(sd::Texture::create(
        m_target.getWidth(), m_target.getHeight(), 1, sd::TextureType::TEX_2D,
        sd::TextureFormat::RGBA, sd::TextureFormatType::FLOAT,
        sd::TextureWrap::BORDER, sd::TextureFilter::LINEAR,
        sd::TextureMipmapFilter::LINEAR_NEAREST));
    m_framebuffer->attachTexture(sd::Texture::create(
        m_target.getWidth(), m_target.getHeight(), 1, sd::TextureType::TEX_2D,
        sd::TextureFormat::ALPHA, sd::TextureFormatType::UINT,
        sd::TextureWrap::BORDER, sd::TextureFilter::NEAREST,
        sd::TextureMipmapFilter::NEAREST));
    m_framebuffer->setDrawable({0, 1});

    show();
}

void EditorLayer::onDetech() { hide(); }

void EditorLayer::onRender() {}

void EditorLayer::onTick(float dt) {
    auto [mouseX, mouseY] = ImGui::GetMousePos();
    mouseX -= m_viewportBounds[0].x;
    mouseY -= m_viewportBounds[0].y;
    glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
    if (!ImGuizmo::IsUsing() && ImGui::IsMouseDown(0) && m_isViewportHovered) {
        sd::EntityId id =
            m_framebuffer->readPixels(1, mouseX, viewportSize.y - mouseY);
        if (id != sd::Entity::INVALID_ID) {
            m_scenePanel.setSelectedEntity(id);
        }
    }
    sd::Entity entity = m_scenePanel.getSelectedEntity();
    if (entity) {
        glm::vec3 pos = entity.getComponent<sd::TransformComponent>()
                            .transform.getWorldPosition();
        m_cameraController.setFocus(pos);
    }
    m_cameraController.tick(dt);
}

void EditorLayer::onImGui() {
    if (m_hide) {
        return;
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
                newScene();
            }

            if (ImGui::MenuItem("Load Scene...", "Ctrl+L")) {
                loadScene();
            }

            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
                saveScene();
            }

            if (ImGui::MenuItem("Exit", "Esc")) {
                sd::Application::quit();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    m_scenePanel.onImGui();

    ImGui::Begin("Camera");
    {
        glm::vec3 position = m_editorCamera.getWorldPosition();
        if (ImGui::DrawVec3Control("Translation", position)) {
            m_editorCamera.setWorldPosition(position);
        }

        glm::vec3 rotation =
            glm::degrees(glm::eulerAngles(m_editorCamera.getWorldRotation()));
        if (ImGui::DrawVec3Control("Rotation", rotation)) {
            m_editorCamera.setWorldRotation(glm::radians(rotation));
        }
    }
    ImGui::End();

    ImGui::Begin("Render Settings");
    {
        float exposure = sd::Application::getRenderEngine().getExposure();
        if (ImGui::SliderFloat("Exposure", &exposure, 0, 10)) {
            sd::Application::getRenderEngine().setExposure(exposure);
        }

        bool isBloom = sd::Application::getRenderEngine().getBloom();
        if (ImGui::Checkbox("Bloom", &isBloom)) {
            sd::Application::getRenderEngine().setBloom(isBloom);
        }
        float bloom = sd::Application::getRenderEngine().getBloomFactor();
        if (ImGui::SliderFloat("Bloom Factor", &bloom, 0.1, 1)) {
            sd::Application::getRenderEngine().setBloomFactor(bloom);
        }
    }
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("GBuffer");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        for (int i = 0; i < 4; ++i) {
            ImGui::DrawTexture(*sd::Application::getRenderEngine()
                                    .getRenderSystem()
                                    ->getGBuffer()
                                    ->getTexture(i),
                               wsize);
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
                sd::Application::getRenderEngine().resize(wsize.x, wsize.y);

                m_framebuffer->resize(wsize.x, wsize.y);
            }

            m_width = wsize.x;
            m_height = wsize.y;
        }
        // Copy the multisample texture to normal texture
        m_framebuffer->copyFrom(m_target.getFramebuffer(),
                                sd::BufferBit::COLOR_BUFFER_BIT,
                                sd::TextureFilter::NEAREST);

        m_isViewportFocused = ImGui::IsWindowFocused();
        m_isViewportHovered = ImGui::IsWindowHovered();
        ImGui::DrawTexture(*m_framebuffer->getTexture(), wsize);

        sd::Entity selectedEntity = m_scenePanel.getSelectedEntity();
        if (selectedEntity) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(m_viewportBounds[0].x, m_viewportBounds[0].y,
                              m_viewportBounds[1].x - m_viewportBounds[0].x,
                              m_viewportBounds[1].y - m_viewportBounds[0].y);
            const glm::mat4 &view = m_editorCamera.getView();
            const glm::mat4 &projection = m_editorCamera.getProjection();

            auto &tc = selectedEntity.getComponent<sd::TransformComponent>();
            glm::mat4 transform = tc.transform.getWorldTransform();
            ImGuizmo::OPERATION op =
                static_cast<ImGuizmo::OPERATION>(m_scenePanel.getGizmoType());
            ImGuizmo::MODE mode =
                static_cast<ImGuizmo::MODE>(m_scenePanel.getGizmoMode());
            if (ImGuizmo::Manipulate(
                    glm::value_ptr(view), glm::value_ptr(projection), op, mode,
                    glm::value_ptr(transform), nullptr, nullptr)) {
                tc.transform.setWorldTransform(transform);
            }
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();

    processDialog();
}

void EditorLayer::hide() {
    m_hide = true;
    setBlockEvent(false);
    int w = sd::Application::getWindow().getWidth();
    int h = sd::Application::getWindow().getHeight();
    sd::Application::getRenderEngine().setRenderTarget(nullptr);
    sd::Application::getRenderEngine().resize(w, h);
}

void EditorLayer::show() {
    m_hide = false;
    setBlockEvent(true);
    sd::Application::getRenderEngine().setCamera(&m_editorCamera);
    sd::Application::getRenderEngine().setRenderTarget(&m_target);
    sd::Application::getRenderEngine().resize(m_width, m_height);
}

void EditorLayer::onEventProcess(const SDL_Event &event) {
    if (m_isViewportFocused && m_isViewportHovered) {
        m_cameraController.processEvent(event);
    }
    bool lshift = sd::Input::isKeyDown(SDLK_LSHIFT);
    bool lctrl = sd::Input::isKeyDown(SDLK_LCTRL);
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_z: {
                if (lctrl) {
                    if (m_hide) {
                        show();
                    } else {
                        hide();
                    }
                }
            } break;
            case SDLK_s: {
                if (lshift && lctrl) {
                    saveScene();
                }
            } break;
            case SDLK_n: {
                if (lctrl) {
                    newScene();
                }
            } break;
            case SDLK_l: {
                if (lctrl) {
                    loadScene();
                }
            } break;
            case SDLK_ESCAPE: {
                sd::Application::quit();
            } break;
        }
    }
}

void EditorLayer::onEventsProcess() {
    if (m_isViewportFocused && m_isViewportHovered) {
        m_cameraController.processEvents();
    }
}

void EditorLayer::newScene() {
    m_scene = sd::createRef<sd::Scene>();
    m_scenePanel.setScene(m_scene.get());
    sd::Application::getRenderEngine().setScene(m_scene.get());
}

void EditorLayer::loadScene() {
    m_loadSceneOpen = true;
    m_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
    m_fileDialogInfo.title = "Load Scene";
    m_fileDialogInfo.fileName = "";
    m_fileDialogInfo.fileExtension = ".scene";
    m_fileDialogInfo.directoryPath = std::filesystem::current_path();
}

void EditorLayer::saveScene() {
    m_saveSceneOpen = true;
    m_fileDialogInfo.type = ImGuiFileDialogType_SaveFile;
    m_fileDialogInfo.title = "Save Scene";
    m_fileDialogInfo.fileName = "test.scene";
    m_fileDialogInfo.fileExtension = ".scene";
    m_fileDialogInfo.directoryPath = std::filesystem::current_path();
}

void EditorLayer::processDialog() {
    if (ImGui::FileDialog(&m_loadSceneOpen, &m_fileDialogInfo)) {
        m_scene->load(m_fileDialogInfo.resultPath.string());
    }
    if (ImGui::FileDialog(&m_saveSceneOpen, &m_fileDialogInfo)) {
        m_scene->save(m_fileDialogInfo.resultPath.string());
    }
}
