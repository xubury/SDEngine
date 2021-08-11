#include "ScenePanel.hpp"
#include "ECS/Component.hpp"
#include "imgui.h"

ScenePanel::ScenePanel() : m_scene(sd::createRef<sd::Scene>()) {}

ScenePanel::ScenePanel(const sd::Ref<sd::Scene> &scene) : m_scene(scene) {}

void ScenePanel::setScene(const sd::Ref<sd::Scene> &scene) { m_scene = scene; }

void ScenePanel::onImGui() {
    if (m_scene == nullptr) {
        return;
    }

    ImGui::Begin("Scene Hierarchy");

    m_scene->getRegistry().each([&](auto entityID) {
        sd::Entity entity{entityID, m_scene.get()};
        drawEntityNode(entity);
    });

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        m_selectedEntity = {};

    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
        if (ImGui::MenuItem("Create Empty Entity"))
            m_scene->createEntity("Empty Entity");

        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Properties");
    if (m_selectedEntity) {
        drawComponents(m_selectedEntity);
    }

    ImGui::End();
}

void ScenePanel::drawEntityNode(sd::Entity &entity) {
    auto &tag = entity.getComponent<sd::TagComponent>().tag;

    ImGuiTreeNodeFlags flags =
        ((m_selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
        ImGuiTreeNodeFlags_OpenOnArrow;
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags,
                                    tag.c_str());
    if (ImGui::IsItemClicked()) {
        m_selectedEntity = entity;
    }

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Delete Entity")) entityDeleted = true;

        ImGui::EndPopup();
    }

    if (opened) {
        // ImGuiTreeNodeFlags flags =
        //     ImGuiTreeNodeFlags_OpenOnArrow |
        //     ImGuiTreeNodeFlags_SpanAvailWidth;
        // bool opened = ImGui::TreeNodeEx((void *)9817239, flags, tag.c_str());
        // if (opened) ImGui::TreePop();
        ImGui::TreePop();
    }

    if (entityDeleted) {
        m_scene->destroyEntity(entity);
        if (m_selectedEntity == entity) m_selectedEntity = {};
    }
}

void ScenePanel::drawComponents(sd::Entity &entity) {
    if (entity.hasComponent<sd::TagComponent>()) {
        auto &tag = entity.getComponent<sd::TagComponent>().tag;

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        std::strncpy(buffer, tag.c_str(), tag.size());
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
            tag = std::string(buffer);
        }
    }
}
