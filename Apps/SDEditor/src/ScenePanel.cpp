#include "ScenePanel.hpp"
#include "Core/Application.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGuizmo.h"

ScenePanel::ScenePanel()
    : m_scene(nullptr),
      m_fileDialogOpen(false),
      m_gizmoMode(ImGuizmo::WORLD),
      m_gizmoType(ImGuizmo::TRANSLATE) {}

ScenePanel::ScenePanel(sd::Scene *scene) : m_scene(scene) {}

void ScenePanel::setScene(sd::Scene *scene) { m_scene = scene; }

void ScenePanel::setSelectedEntity(sd::EntityId entityId) {
    m_selectedEntity = sd::Entity(entityId, m_scene);
}

sd::Entity ScenePanel::getSelectedEntity() const { return m_selectedEntity; }

int ScenePanel::getGizmoMode() const { return m_gizmoMode; }

int ScenePanel::getGizmoType() const { return m_gizmoType; }

void ScenePanel::onImGui() {
    if (m_scene == nullptr) {
        return;
    }

    ImGui::Begin("Scene Hierarchy");

    m_scene->each([&](auto entityID) {
        sd::Entity entity{entityID, m_scene};

        sd::EntityDataComponent &data =
            entity.getComponent<sd::EntityDataComponent>();
        sd::Entity parent(data.m_parent, m_scene);
        if (!parent) {
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
    sd::EntityDataComponent &data =
        entity.getComponent<sd::EntityDataComponent>();

    auto &tag = entity.getComponent<sd::TagComponent>().tag;

    static ImGuiTreeNodeFlags base_flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth;
    static ImGuiTreeNodeFlags leaf_flags =
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags flags =
        data.m_children.empty() ? leaf_flags : base_flags;
    flags |= ((m_selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
             ImGuiTreeNodeFlags_OpenOnArrow;
    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(sd::EntityId)entity,
                                    flags, tag.c_str());
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
        for (entt::entity childId : data.m_children) {
            sd::Entity child(childId, m_scene);
            drawEntityNode(child);
        }
        ImGui::TreePop();
    }
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
        bool removeComponent = false;
        if (removeable) {
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button("+", ImVec2{lineHeight, lineHeight})) {
                ImGui::OpenPopup("ComponentSettings");
            }

            if (ImGui::BeginPopup("ComponentSettings")) {
                if (ImGui::MenuItem("Remove component")) {
                    removeComponent = true;
                }
                ImGui::EndPopup();
            }
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
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");

    if (ImGui::BeginPopup("AddComponent")) {
        if (ImGui::MenuItem("Model")) {
            if (!m_selectedEntity.hasComponent<sd::ModelComponent>())
                m_selectedEntity.addComponent<sd::ModelComponent>();
            else
                SD_CORE_WARN("This entity already has the Model Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Terrain")) {
            if (!m_selectedEntity.hasComponent<sd::TerrainComponent>())
                m_selectedEntity.addComponent<sd::TerrainComponent>();
            else
                SD_CORE_WARN("This entity already has the Terrain Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Light")) {
            if (!m_selectedEntity.hasComponent<sd::LightComponent>())
                m_selectedEntity.addComponent<sd::LightComponent>();
            else
                SD_CORE_WARN("This entity already has the Light Component!");
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopItemWidth();

    drawComponent<sd::TransformComponent>(
        "Transform", entity,
        [&](sd::TransformComponent &component) {
            ImGui::RadioButton("World", &m_gizmoMode, ImGuizmo::WORLD);
            ImGui::SameLine();
            ImGui::RadioButton("Local", &m_gizmoMode, ImGuizmo::LOCAL);

            ImGui::RadioButton("Translate", &m_gizmoType, ImGuizmo::TRANSLATE);
            ImGui::SameLine();
            ImGui::RadioButton("Rotate", &m_gizmoType, ImGuizmo::ROTATE);
            ImGui::SameLine();
            ImGui::RadioButton("Scale", &m_gizmoType, ImGuizmo::SCALE);
            glm::vec3 position = component.transform.getWorldPosition();
            if (ImGui::DrawVec3Control("Translation", position)) {
                component.transform.setWorldPosition(position);
            }

            glm::vec3 rotation = glm::degrees(
                glm::eulerAngles(component.transform.getWorldRotation()));
            if (ImGui::DrawVec3Control("Rotation", rotation)) {
                component.transform.setWorldRotation(glm::radians(rotation));
            }

            glm::vec3 scale = component.transform.getWorldScale();
            if (ImGui::DrawVec3Control("Scale", scale, 1.0f)) {
                component.transform.setWorldScale(scale);
            }
        },
        false);
    drawComponent<sd::ModelComponent>(
        "Model", entity, [&](sd::ModelComponent &model) {
            ImGui::InputText("##Path", model.path.data(), model.path.size(),
                             ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            if (ImGui::Button("Open")) {
                m_fileDialogOpen = true;
                m_fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
                m_fileDialogInfo.title = "Open File";
                m_fileDialogInfo.fileName = "";
                m_fileDialogInfo.directoryPath =
                    std::filesystem::current_path();
            }
            if (ImGui::FileDialog(&m_fileDialogOpen, &m_fileDialogInfo)) {
                model.path = m_fileDialogInfo.resultPath.string();
                model.model = sd::Graphics::assetManager().load<sd::Model>(
                    m_fileDialogInfo.resultPath);
            }

            ImVec2 size(64, 64);
            drawMaterial(model.model->getMaterial(), size);
        });
    drawComponent<sd::TerrainComponent>(
        "Terrain", entity, [&](sd::TerrainComponent &terrain) {
            int gridSize = terrain.terrain.getGridSize();
            int vertexCount = terrain.terrain.getVertexCount();
            if (ImGui::InputInt("Grid size:", &gridSize)) {
                terrain.terrain.setGridSize(std::max(gridSize, 1));
                terrain.terrain.generateMesh();
                sd::Application::getRenderEngine()
                    .getTerrainSystem()
                    ->updateTerrain(entity);
            }
            if (ImGui::InputInt("Vertex count:", &vertexCount)) {
                terrain.terrain.setVertexCount(std::max(vertexCount, 2));
                terrain.terrain.generateMesh();
                sd::Application::getRenderEngine()
                    .getTerrainSystem()
                    ->updateTerrain(entity);
            }
        });
    drawComponent<sd::LightComponent>(
        "Light", entity, [&](sd::LightComponent &light) {
            ImGui::ColorEdit3("Diffuse", &light.light.diffuse[0]);
            ImGui::ColorEdit3("Ambient", &light.light.ambient[0]);
            ImGui::ColorEdit3("Specular", &light.light.specular[0]);
        });
}

void ScenePanel::drawMaterial(sd::Material &material, const ImVec2 &size) {
    for (auto &[type, texture] : material.getTextures()) {
        ImGui::DrawTexture(*texture.texture, size);
    }
}
