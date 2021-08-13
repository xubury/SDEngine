#include "ScenePanel.hpp"
#include "ECS/Component.hpp"
#include "imgui.h"
#include "Core/Transform.hpp"
#include <stack>

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

        sd::EntityDataComponent &data =
            entity.getComponent<sd::EntityDataComponent>();
        if (!data.m_parent) {
            drawEntityNode(entity);
        }
    });

    if (m_destroyEntity) {
        m_scene->destroyEntity(m_destroyEntity);
        if (m_selectedEntity == m_destroyEntity) m_selectedEntity = {};
        m_destroyEntity = {};
    }

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
    if (ImGui::IsItemClicked(0)) {
        m_selectedEntity = entity;
    }

    if (ImGui::BeginPopupContextItem()) {
        m_selectedEntity = entity;
        if (ImGui::MenuItem("Delete Entity")) {
            m_destroyEntity = entity;
        };
        if (ImGui::MenuItem("Create Empty Entity")) {
            sd::Entity newEntity = m_scene->createEntity("Empty Entity");
            m_scene->addChildToEntity(entity, newEntity);
        }

        ImGui::EndPopup();
    }

    if (opened) {
        sd::EntityDataComponent &data =
            entity.getComponent<sd::EntityDataComponent>();

        for (sd::Entity child : data.m_children) {
            drawEntityNode(child);
        }
        ImGui::TreePop();
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
