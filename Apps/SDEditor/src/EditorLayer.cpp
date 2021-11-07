#include "EditorLayer.hpp"
#include "Core/Application.hpp"
#include "Input/InputEngine.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/RenderEngine.hpp"
#include "ECS/Component.hpp"
#include "Utility/Image.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "ImGui/ImGuiWidget.hpp"

EditorLayer::EditorLayer(int width, int height)
    : sd::Layer("Editor Layer"),
      m_width(width),
      m_height(height),
      m_isViewportFocused(false),
      m_isViewportHovered(false),
      m_hide(false),
      m_editorCamera(glm::radians(45.f), width, height, 0.1, 1000.f),
      m_loadSceneOpen(false),
      m_saveSceneOpen(false) {
    m_cameraController.setCamera(&m_editorCamera);
    m_editorCamera.setWorldPosition(glm::vec3(0, 0, 10));
    auto image = sd::Asset::manager().load<sd::Image>("icons/light.png");

    m_lightIcon = sd::Texture::create(
        image->width(), image->height(), 1, sd::TextureType::TEX_2D,
        image->hasAlpha() ? sd::TextureFormat::RGBA : sd::TextureFormat::RGB,
        sd::TextureFormatType::UBYTE, sd::TextureWrap::REPEAT,
        sd::TextureFilter::LINEAR, sd::TextureMipmapFilter::LINEAR_LINEAR,
        image->data());

    newScene();
    m_screenBuffer = sd::Framebuffer::create();
    m_screenBuffer->attachTexture(sd::Texture::create(
        m_width, m_height, 1, sd::TextureType::TEX_2D, sd::TextureFormat::RGBA,
        sd::TextureFormatType::UBYTE, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST));

    m_debugGBuffer = sd::Framebuffer::create();
    for (int i = 0; i < sd::GeometryBufferType::GBUFFER_COUNT; ++i) {
        m_debugGBuffer->attachTexture(sd::Texture::create(
            width, height, 1, sd::TextureType::TEX_2D,
            getTextureFormat(sd::GeometryBufferType(i)),
            getTextureFormatType(sd::GeometryBufferType(i)),
            sd::TextureWrap::EDGE, sd::TextureFilter::NEAREST,
            sd::TextureMipmapFilter::NEAREST));
    }
}

void EditorLayer::onAttach() { show(); }

void EditorLayer::onDetech() { hide(); }

void EditorLayer::onRender() {
    if (m_hide) return;

    sd::Device::instance().setRenderTarget(sd::RenderEngine::getRenderTarget());
    sd::Device::instance().clear(sd::BufferBitMask::DEPTH_BUFFER_BIT);
    sd::Renderer::beginScene(m_editorCamera);
    auto lightView =
        m_scene->view<sd::LightComponent, sd::TransformComponent>();
    lightView.each([this](const sd::LightComponent &,
                          const sd::TransformComponent &transComp) {
        glm::vec3 pos = transComp.transform.getWorldPosition();
        float dist = glm::distance(pos, m_editorCamera.getWorldPosition());
        float scale = (dist - m_editorCamera.getNearZ()) / 20;
        sd::Renderer::drawBillboard(m_lightIcon, pos, glm::vec2(scale));
    });
    sd::Renderer::endScene();
}

void EditorLayer::onTick(float dt) {
    auto [mouseX, mouseY] = ImGui::GetMousePos();
    mouseX -= m_viewportBounds[0].x;
    mouseY -= m_viewportBounds[0].y;
    glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
    if (!ImGuizmo::IsUsing() && !m_hide && ImGui::IsMouseDown(0) &&
        m_isViewportHovered) {
        entt::entity entity = sd::Entity::INVALID_ID;
        m_debugGBuffer->readPixels(sd::GeometryBufferType::G_ENTITY_ID, 0,
                                   mouseX, viewportSize.y - mouseY, 0, 1, 1, 1,
                                   sizeof(entity), &entity);
        if (entity != sd::Entity::INVALID_ID) {
            m_scenePanel.setSelectedEntity({entity, m_scene.get()});
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
    sd::Device::instance().blitFramebuffer(nullptr, 0, m_screenBuffer.get(), 0,
                                           sd::BufferBitMask::COLOR_BUFFER_BIT,
                                           sd::TextureFilter::NEAREST);
    for (int i = 0; i < sd::GeometryBufferType::GBUFFER_COUNT; ++i) {
        sd::Device::instance().blitFramebuffer(
            sd::RenderEngine::getLightingSystem()->getGBuffer(), i,
            m_debugGBuffer.get(), i, sd::BufferBitMask::COLOR_BUFFER_BIT,
            sd::TextureFilter::NEAREST);
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

    ImGui::Begin("Render Settings");
    {
        float exposure = sd::RenderEngine::getExposure();
        if (ImGui::SliderFloat("Exposure", &exposure, 0, 10)) {
            sd::RenderEngine::setExposure(exposure);
        }

        bool isBloom = sd::RenderEngine::getBloom();
        if (ImGui::Checkbox("Bloom", &isBloom)) {
            sd::RenderEngine::setBloom(isBloom);
        }
        float bloom = sd::RenderEngine::getBloomFactor();
        if (ImGui::SliderFloat("Bloom Factor", &bloom, 0.1, 1)) {
            sd::RenderEngine::setBloomFactor(bloom);
        }
    }
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("GBuffer");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        for (int i = 0; i < sd::GeometryBufferType::G_ENTITY_ID; ++i) {
            ImGui::DrawTexture(*m_debugGBuffer->getTexture(i), wsize);
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
                sd::RenderEngine::resize(wsize.x, wsize.y);
                m_screenBuffer->resize(wsize.x, wsize.y);
                m_debugGBuffer->resize(wsize.x, wsize.y);
            }

            m_width = wsize.x;
            m_height = wsize.y;
        }
        m_isViewportFocused = ImGui::IsWindowFocused();
        m_isViewportHovered = ImGui::IsWindowHovered();
        ImGui::DrawTexture(*m_screenBuffer->getTexture(), wsize);

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
    int w = sd::Window::getWidth();
    int h = sd::Window::getHeight();
    sd::RenderEngine::resize(w, h);
}

void EditorLayer::show() {
    m_hide = false;
    setBlockEvent(true);
    sd::RenderEngine::setCamera(&m_editorCamera);
    sd::RenderEngine::resize(m_width, m_height);
}

void EditorLayer::onEventProcess(const SDL_Event &event) {
    if (m_isViewportHovered) {
        m_cameraController.processEvent(event);
    }
    bool lshift = sd::InputEngine::isKeyDown(SDLK_LSHIFT);
    bool lctrl = sd::InputEngine::isKeyDown(SDLK_LCTRL);
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
    if (m_isViewportHovered) {
        m_cameraController.processEvents();
    }
}

void EditorLayer::newScene() {
    m_scene = sd::createRef<sd::Scene>();
    m_scenePanel.setScene(m_scene.get());
    sd::RenderEngine::setScene(m_scene.get());
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
