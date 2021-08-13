#include "ScenePanel.hpp"
#include "ECS/Component.hpp"
#include "imgui.h"
#include "imgui_internal.h"

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

static void drawVec3Control(const std::string &label, glm::vec3 &values,
                            float resetValue = 0.0f,
                            float columnWidth = 100.0f) {
    ImGuiIO &io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

    float lineHeight =
        GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize)) values.x = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize)) values.y = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize)) values.z = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}

template <typename T, typename UIFunction>
static void drawComponent(const std::string &name, sd::Entity entity,
                          UIFunction uiFunction, bool removeable = true) {
    const ImGuiTreeNodeFlags treeNodeFlags =
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth |
        ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
    if (entity.hasComponent<T>()) {
        auto &component = entity.getComponent<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        float lineHeight =
            GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx((void *)typeid(T).hash_code(),
                                      treeNodeFlags, name.c_str());
        ImGui::PopStyleVar();
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("+", ImVec2{lineHeight, lineHeight})) {
            ImGui::OpenPopup("ComponentSettings");
        }

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings")) {
            if (removeable && ImGui::MenuItem("Remove component"))
                removeComponent = true;

            ImGui::EndPopup();
        }

        if (open) {
            uiFunction(component);
            ImGui::TreePop();
        }

        if (removeComponent) entity.removeComponent<T>();
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
    drawComponent<sd::TransformComponent>(
        "Transform", entity,
        [](sd::TransformComponent &component) {
            ImGui::Text("Local");

            glm::vec3 position = component.transform.getLocalPosition();
            drawVec3Control("Translation", position);
            component.transform.setLocalPosition(position);

            glm::vec3 rotation =
                glm::degrees(component.transform.getLocalEulerAngle());
            drawVec3Control("Rotation", rotation);
            component.transform.setLocalRotation(glm::radians(rotation));

            glm::vec3 scale = component.transform.getLocalScale();
            drawVec3Control("Scale", scale, 1.0f);
            component.transform.setLocalScale(scale);

            // world transform
            ImGui::Text("World");

            position = component.transform.getWorldPosition();
            drawVec3Control("Translation", position);
            component.transform.setWorldPosition(position);

            rotation = glm::degrees(component.transform.getWorldEulerAngle());
            drawVec3Control("Rotation", rotation);
            // component.transform.setWorldRotation(glm::radians(rotation));

            scale = component.transform.getWorldScale();
            drawVec3Control("Scale", scale, 1.0f);
            component.transform.setWorldScale(scale);
        },
        false);
}
