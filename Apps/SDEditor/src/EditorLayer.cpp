#include "EditorLayer.hpp"
#include "Core/Application.hpp"
#include "Input/Input.hpp"
#include "Renderer/Renderer.hpp"
#include "ECS/Component.hpp"
#include "Asset/Image.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "ImGui/ImGuiWidget.hpp"

namespace sd {

EditorLayer::EditorLayer(int width, int height)
    : Layer("Editor Layer"),
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
    auto image = AssetManager::instance().get<Image>("icons/light.png");

    m_lightIcon = Texture::create(
        image->width(), image->height(), 1, TextureType::TEX_2D,
        image->hasAlpha() ? TextureFormat::RGBA : TextureFormat::RGB,
        TextureFormatType::UBYTE, TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_LINEAR, image->data());

    newScene();
    m_screenBuffer = Framebuffer::create();
    m_screenBuffer->attachTexture(Texture::create(
        m_width, m_height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::UBYTE, TextureWrap::BORDER, TextureFilter::NEAREST,
        TextureMipmapFilter::NEAREST));

    m_debugGBuffer = Framebuffer::create();
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        m_debugGBuffer->attachTexture(Texture::create(
            width, height, 1, TextureType::TEX_2D,
            getTextureFormat(GeometryBufferType(i)),
            getTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
            TextureFilter::NEAREST, TextureMipmapFilter::NEAREST));
    }
}

void EditorLayer::onAttach() { show(); }

void EditorLayer::onDetech() { hide(); }

void EditorLayer::onRender() {
    if (m_hide) return;

    Renderer::engine().getRenderTarget().bind();
    Renderer::device().clear(BufferBitMask::DEPTH_BUFFER_BIT);
    Renderer2D::beginScene(m_editorCamera);
    auto lightView = m_scene->view<LightComponent, TransformComponent>();
    lightView.each(
        [this](const LightComponent &, const TransformComponent &transComp) {
            glm::vec3 pos = transComp.transform.getWorldPosition();
            float dist = glm::distance(pos, m_editorCamera.getWorldPosition());
            float scale = (dist - m_editorCamera.getNearZ()) / 20;
            Renderer2D::drawBillboard(m_lightIcon, pos, glm::vec2(scale));
        });
    Renderer2D::endScene();
}

void EditorLayer::onTick(float dt) {
    auto [mouseX, mouseY] = ImGui::GetMousePos();
    mouseX -= m_viewportBounds[0].x;
    mouseY -= m_viewportBounds[0].y;
    glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
    if (!ImGuizmo::IsUsing() && !m_hide && ImGui::IsMouseDown(0) &&
        m_isViewportHovered) {
        entt::entity entity = Entity::INVALID_ID;
        m_debugGBuffer->readPixels(GeometryBufferType::G_ENTITY_ID, 0, mouseX,
                                   viewportSize.y - mouseY, 0, 1, 1, 1,
                                   sizeof(entity), &entity);
        if (entity != Entity::INVALID_ID) {
            m_scenePanel.setSelectedEntity({entity, m_scene.get()});
        }
    }
    Entity entity = m_scenePanel.getSelectedEntity();
    if (entity) {
        glm::vec3 pos = entity.getComponent<TransformComponent>()
                            .transform.getWorldPosition();
        m_cameraController.setFocus(pos);
    }
    m_cameraController.tick(dt);
}

void EditorLayer::onImGui() {
    if (m_hide) {
        return;
    }
    Renderer::device().blitFramebuffer(
        Renderer::engine().getRenderTarget().getFramebuffer(), 0,
        m_screenBuffer.get(), 0, BufferBitMask::COLOR_BUFFER_BIT,
        TextureFilter::NEAREST);
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        Renderer::device().blitFramebuffer(
            Renderer::engine().getLightingSystem()->getGBuffer(), i,
            m_debugGBuffer.get(), i, BufferBitMask::COLOR_BUFFER_BIT,
            TextureFilter::NEAREST);
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
                getApp().quit();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    m_scenePanel.onImGui();

    ImGui::Begin("Render Settings");
    {
        float exposure = Renderer::engine().getExposure();
        ImGui::TextUnformatted("Exposure");
        if (ImGui::SliderFloat("##Exposure", &exposure, 0, 10)) {
            Renderer::engine().setExposure(exposure);
        }

        bool isBloom = Renderer::engine().getBloom();
        if (ImGui::Checkbox("Bloom", &isBloom)) {
            Renderer::engine().setBloom(isBloom);
        }
        float bloom = Renderer::engine().getBloomFactor();
        ImGui::TextUnformatted("Bloom Factor");
        if (ImGui::SliderFloat("##Bloom Factor", &bloom, 0.1, 1)) {
            Renderer::engine().setBloomFactor(bloom);
        }

        float gamma = Renderer::engine().getGammaCorrection();
        ImGui::TextUnformatted("Gamma Correction");
        if (ImGui::SliderFloat("##Gamma Correction", &gamma, 0.1, 3)) {
            Renderer::engine().setGammaCorrection(gamma);
        }
    }
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("GBuffer");
    {
        ImVec2 wsize = ImGui::GetContentRegionAvail();
        for (int i = 0; i < GeometryBufferType::G_ENTITY_ID; ++i) {
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
                Renderer::engine().resize(wsize.x, wsize.y);
                m_screenBuffer->resize(wsize.x, wsize.y);
                m_debugGBuffer->resize(wsize.x, wsize.y);
            }

            m_width = wsize.x;
            m_height = wsize.y;
        }
        m_isViewportFocused = ImGui::IsWindowFocused();
        m_isViewportHovered = ImGui::IsWindowHovered();
        ImGui::DrawTexture(*m_screenBuffer->getTexture(), wsize);

        Entity selectedEntity = m_scenePanel.getSelectedEntity();
        if (selectedEntity) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(m_viewportBounds[0].x, m_viewportBounds[0].y,
                              m_viewportBounds[1].x - m_viewportBounds[0].x,
                              m_viewportBounds[1].y - m_viewportBounds[0].y);
            const glm::mat4 &view = m_editorCamera.getView();
            const glm::mat4 &projection = m_editorCamera.getProjection();

            auto &tc = selectedEntity.getComponent<TransformComponent>();
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
    int w = getApp().getWindow().getWidth();
    int h = getApp().getWindow().getHeight();
    Renderer::engine().resize(w, h);
}

void EditorLayer::show() {
    m_hide = false;
    setBlockEvent(true);
    Renderer::engine().setCamera(&m_editorCamera);
    Renderer::engine().resize(m_width, m_height);
}

void EditorLayer::onEventProcess(const SDL_Event &event) {
    if (m_isViewportHovered) {
        m_cameraController.processEvent(event);
    }
    bool lshift = Input::isKeyDown(SDLK_LSHIFT);
    bool lctrl = Input::isKeyDown(SDLK_LCTRL);
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
                getApp().quit();
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
    m_scene = createRef<Scene>();
    m_scenePanel.setScene(m_scene.get());
    Renderer::engine().setScene(m_scene.get());
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

}  // namespace sd
