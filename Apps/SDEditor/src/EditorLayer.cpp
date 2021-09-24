#include "EditorLayer.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Renderer/Renderer.hpp"
#include "Utils/Serialize.hpp"
#include "Core/Application.hpp"
#include "ECS/Component.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"

EditorLayer::EditorLayer()
    : sd::Layer("Editor Layer"),
      m_width(800),
      m_height(600),
      m_isViewportFocused(false),
      m_isViewportHovered(false),
      m_hide(false),
      m_fov(glm::radians(45.f)),
      m_nearZ(0.1f),
      m_farZ(100000.f),
      m_editorCamera(m_fov, static_cast<float>(m_width) / m_height, m_nearZ,
                     m_farZ),
      m_openSceneOpen(false),
      m_saveSceneOpen(false) {
    m_cameraController.setCamera(&m_editorCamera);
    newScene();
}

void EditorLayer::onAttach() {
    sd::Ref<sd::Texture> multisampleTexture = sd::Texture::create(
        1024, 1024, 8, sd::TextureType::TEX_2D_MULTISAMPLE,
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

    m_frameBuffer = sd::Framebuffer::create();
    m_frameBuffer->attachTexture(sd::Texture::create(
        m_target.getWidth(), m_target.getHeight(), 1, sd::TextureType::TEX_2D,
        sd::TextureFormat::RGBA, sd::TextureFormatType::FLOAT,
        sd::TextureWrap::BORDER, sd::TextureFilter::LINEAR,
        sd::TextureMipmapFilter::LINEAR_NEAREST));
    m_frameBuffer->attachTexture(sd::Texture::create(
        m_target.getWidth(), m_target.getHeight(), 1, sd::TextureType::TEX_2D,
        sd::TextureFormat::ALPHA, sd::TextureFormatType::UINT,
        sd::TextureWrap::BORDER, sd::TextureFilter::NEAREST,
        sd::TextureMipmapFilter::NEAREST));
    m_frameBuffer->setDrawable({0, 1});

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
            m_frameBuffer->readPixels(1, mouseX, viewportSize.y - mouseY);
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

            if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) {
                openScene();
            }

            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
                saveScene();
            }

            if (ImGui::MenuItem("Exit")) {
                sd::Application::instance().quit();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    m_scenePanel.onImGui();

    ImGui::Begin("Camera");
    {
        glm::vec3 position = m_editorCamera.getWorldPosition();
        ImGui::DrawVec3Control("Translation", position);
        m_editorCamera.setWorldPosition(position);

        glm::vec3 rotation =
            glm::degrees(glm::eulerAngles(m_editorCamera.getWorldRotation()));
        ImGui::DrawVec3Control("Rotation", rotation);
        m_editorCamera.setWorldRotation(glm::radians(rotation));
    }
    ImGui::End();

    ImGui::Begin("GBuffer");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        for (int i = 0; i < 4; ++i) {
            ImGui::Image((void *)(intptr_t)sd::Application::getRenderEngine()
                             .getRenderSystem()
                             ->getGBuffer()
                             ->getTexture(i)
                             ->getId(),
                         wsize, ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
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

                m_target.resize(wsize.x, wsize.y);
                m_frameBuffer->resize(wsize.x, wsize.y);

                m_editorCamera.setProjection(m_fov, wsize.x / wsize.y, m_nearZ,
                                             m_farZ);
            }

            m_width = wsize.x;
            m_height = wsize.y;
        }
        // Copy the multisample texture to normal texture
        m_frameBuffer->copyFrom(m_target.getFramebuffer(),
                                sd::BufferBit::COLOR_BUFFER_BIT,
                                sd::TextureFilter::NEAREST);

        m_isViewportFocused = ImGui::IsWindowFocused();
        m_isViewportHovered = ImGui::IsWindowHovered();
        ImGui::Image((void *)(intptr_t)m_frameBuffer->getTexture(0)->getId(),
                     wsize, ImVec2(0, 1), ImVec2(1, 0));

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
    sd::Application::getRenderEngine().setRenderTarget(
        &sd::Renderer::getDefaultTarget());
    m_editorCamera.setProjection(
        m_fov, sd::Renderer::getDefaultTarget().getAspect(), m_nearZ, m_farZ);
}

void EditorLayer::show() {
    m_hide = false;
    setBlockEvent(true);
    sd::Application::getRenderEngine().setCamera(&m_editorCamera);
    sd::Application::getRenderEngine().setRenderTarget(&m_target);
    m_editorCamera.setProjection(m_fov, static_cast<float>(m_width) / m_height,
                                 m_nearZ, m_farZ);
}

void EditorLayer::onEventProcess(const SDL_Event &event) {
    if (m_isViewportFocused && m_isViewportHovered) {
        m_cameraController.processEvent(event);
    }
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_z: {
                if (m_hide) {
                    show();
                } else {
                    hide();
                }
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

void EditorLayer::openScene() {
    m_openSceneOpen = true;
    m_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
    m_fileDialogInfo.title = "Open Scene";
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
    if (ImGui::FileDialog(&m_openSceneOpen, &m_fileDialogInfo)) {
        m_scene->clear();
        std::ifstream os(m_fileDialogInfo.resultPath);
        cereal::BinaryInputArchive archive(os);
        entt::snapshot_loader{*m_scene}
            .entities(archive)
            .component<sd::EntityDataComponent, sd::TagComponent,
                       sd::TransformComponent, sd::ModelComponent,
                       sd::LightComponent>(archive);
        m_scene->refresh();
    }
    if (ImGui::FileDialog(&m_saveSceneOpen, &m_fileDialogInfo)) {
        std::ofstream os(m_fileDialogInfo.resultPath);
        cereal::BinaryOutputArchive archive(os);
        entt::snapshot{*m_scene}
            .entities(archive)
            .component<sd::EntityDataComponent, sd::TagComponent,
                       sd::TransformComponent, sd::ModelComponent,
                       sd::LightComponent>(archive);
    }
}
