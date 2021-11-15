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
    m_cameraController.setCamera(&m_editorCamera);
    m_editorCamera.setWorldPosition(glm::vec3(0, 0, 10));
    m_screenBuffer = Framebuffer::create();
    m_screenBuffer->attachTexture(Texture::create(
        m_width, m_height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::UBYTE, TextureWrap::BORDER, TextureFilter::NEAREST,
        TextureMipmapFilter::NEAREST));
    m_debugGBuffer = Framebuffer::create();
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        m_debugGBuffer->attachTexture(Texture::create(
            m_width, m_height, 1, TextureType::TEX_2D,
            getTextureFormat(GeometryBufferType(i)),
            getTextureFormatType(GeometryBufferType(i)), TextureWrap::EDGE,
            TextureFilter::NEAREST, TextureMipmapFilter::NEAREST));
    }
    m_shadowSystem = new ShadowSystem();
    m_lightingSystem = new LightingSystem(&m_target, m_width, m_height, 4);
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

void EditorLayer::onPush() {
    newScene();

    auto resourceId = asset->loadAsset<Image>("icons/light.png");
    auto image = asset->get<Image>(resourceId);

    m_lightIcon = Texture::create(
        image->width(), image->height(), 1, TextureType::TEX_2D,
        image->hasAlpha() ? TextureFormat::RGBA : TextureFormat::RGB,
        TextureFormatType::UBYTE, TextureWrap::REPEAT, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_LINEAR, image->data());

    m_scenePanel.setAppVars(makeAppVars());

    pushSystem(m_shadowSystem);
    pushSystem(m_lightingSystem);
    pushSystem(m_skyboxSystem);
    pushSystem(m_spriteSystem);
    pushSystem(m_postProcessSystem);
    pushSystem(m_profileSystem);

    show();
}

void EditorLayer::onPop() {
    popSystem(m_shadowSystem);
    popSystem(m_lightingSystem);
    popSystem(m_skyboxSystem);
    popSystem(m_spriteSystem);
    popSystem(m_postProcessSystem);
    popSystem(m_profileSystem);
}

void EditorLayer::onRender() {
    renderer->setRenderTarget(m_target);
    renderer->device().clear();
    for (auto &system : getSystems()) {
        system->onRender();
    }

    if (m_hide) return;

    renderer->beginScene(m_editorCamera);
    auto lightView = m_scene->view<LightComponent, TransformComponent>();
    lightView.each(
        [this](const LightComponent &, const TransformComponent &transComp) {
            glm::vec3 pos = transComp.transform.getWorldPosition();
            float dist = glm::distance(pos, m_editorCamera.getWorldPosition());
            float scale = (dist - m_editorCamera.getNearZ()) / 20;
            renderer->drawBillboard(m_lightIcon, pos, glm::vec2(scale));
        });
    renderer->endScene();
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
    renderer->device().blitFramebuffer(
        m_target.getFramebuffer(), 0, m_screenBuffer.get(), 0,
        BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::NEAREST);
    for (int i = 0; i < GeometryBufferType::GBUFFER_COUNT; ++i) {
        renderer->device().blitFramebuffer(
            m_lightingSystem->getGBuffer(), i, m_debugGBuffer.get(), i,
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
        float exposure = m_postProcessSystem->getExposure();
        ImGui::TextUnformatted("Exposure");
        if (ImGui::SliderFloat("##Exposure", &exposure, 0, 10)) {
            m_postProcessSystem->setExposure(exposure);
        }

        bool isBloom = m_postProcessSystem->getBloom();
        if (ImGui::Checkbox("Bloom", &isBloom)) {
            m_postProcessSystem->setBloom(isBloom);
        }
        float bloom = m_postProcessSystem->getBloomFactor();
        ImGui::TextUnformatted("Bloom Factor");
        if (ImGui::SliderFloat("##Bloom Factor", &bloom, 0.1, 1)) {
            m_postProcessSystem->setBloomFactor(bloom);
        }

        float gamma = m_postProcessSystem->getGammaCorrection();
        ImGui::TextUnformatted("Gamma Correction");
        if (ImGui::SliderFloat("##Gamma Correction", &gamma, 0.1, 3)) {
            m_postProcessSystem->setGammaCorrection(gamma);
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
                m_editorCamera.resize(wsize.x, wsize.y);
                m_target.resize(wsize.x, wsize.y);
                m_screenBuffer->resize(wsize.x, wsize.y);
                m_debugGBuffer->resize(wsize.x, wsize.y);
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
}

void EditorLayer::show() {
    m_hide = false;
    setBlockEvent(true);
    renderer->setCamera(&m_editorCamera);
}

void EditorLayer::onEventProcess(const Event &event) {
    if (m_isViewportHovered) {
        m_cameraController.processEvent(event);
    }
    if (event.type == Event::EventType::KEY_PRESSED) {
        switch (event.key.keycode) {
            default:
                break;
            case Keycode::z: {
                if (event.key.mod == Keymod::LCTRL) {
                    if (m_hide) {
                        show();
                    } else {
                        hide();
                    }
                }
            } break;
            case Keycode::s: {
                if (event.key.mod == (Keymod::LCTRL | Keymod::LSHIFT)) {
                    saveScene();
                }
            } break;
            case Keycode::n: {
                if (event.key.mod == Keymod::LCTRL) {
                    newScene();
                }
            } break;
            case Keycode::l: {
                if (event.key.mod == Keymod::LCTRL) {
                    loadScene();
                }
            } break;
            case Keycode::ESCAPE: {
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
    renderer->setScene(m_scene.get());
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

}  // namespace SD
