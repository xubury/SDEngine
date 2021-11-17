#include "ScenePanel.hpp"
#include "Renderer/Renderer.hpp"
#include "Utility/String.hpp"
#include "Asset/Asset.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGuizmo.h"

namespace SD {

ScenePanel::ScenePanel()
    : m_scene(nullptr),
      m_gizmoMode(ImGuizmo::WORLD),
      m_gizmoType(ImGuizmo::TRANSLATE) {}

ScenePanel::ScenePanel(Scene *scene) : m_scene(scene) {}

void ScenePanel::Reset() {
    m_selectedEntity = {};
    m_selectedMaterialIdMap.clear();
}

void ScenePanel::SetScene(Scene *scene) {
    if (m_scene != scene) {
        Reset();
    }
    m_scene = scene;
}

void ScenePanel::SetSelectedEntity(Entity entity) { m_selectedEntity = entity; }

Entity ScenePanel::GetSelectedEntity() const { return m_selectedEntity; }

int ScenePanel::GetGizmoMode() const { return m_gizmoMode; }

int ScenePanel::GetGizmoType() const { return m_gizmoType; }

void ScenePanel::OnImGui() {
    if (m_scene == nullptr) {
        return;
    }

    ImGui::Begin("Scene Hierarchy");

    m_scene->each([&](auto entityID) {
        Entity entity{entityID, m_scene};

        EntityDataComponent &data = entity.GetComponent<EntityDataComponent>();
        Entity parent(data.parent, m_scene);
        if (!parent) {
            DrawEntityNode(entity);
        }
    });

    if (m_destroyEntity) {
        m_scene->DestroyEntity(m_destroyEntity);
        if (m_selectedEntity == m_destroyEntity) m_selectedEntity = {};
        m_destroyEntity = {};
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        m_selectedEntity = {};

    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
        if (ImGui::MenuItem("Create Empty Entity"))
            m_scene->CreateEntity("Empty Entity");

        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Properties");
    if (m_selectedEntity) {
        DrawComponents(m_selectedEntity);
    }

    ImGui::End();
}

void ScenePanel::DrawEntityNode(Entity &entity) {
    EntityDataComponent &data = entity.GetComponent<EntityDataComponent>();

    auto &tag = entity.GetComponent<TagComponent>().tag;

    static ImGuiTreeNodeFlags base_flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    static ImGuiTreeNodeFlags leaf_flags =
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags flags = data.children.empty() ? leaf_flags : base_flags;
    flags |= ((m_selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
             ImGuiTreeNodeFlags_OpenOnArrow;
    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(entt::entity)entity,
                                    flags, "%s", tag.c_str());
    if (ImGui::IsItemClicked(0)) {
        m_selectedEntity = entity;
    }

    if (ImGui::BeginPopupContextItem()) {
        m_selectedEntity = entity;
        if (ImGui::MenuItem("Delete Entity")) {
            m_destroyEntity = entity;
        };
        if (ImGui::MenuItem("Create Empty Entity")) {
            Entity newEntity = m_scene->CreateEntity("Empty Entity");
            m_scene->AddChildToEntity(entity, newEntity);
        }

        ImGui::EndPopup();
    }

    if (opened) {
        for (entt::entity childId : data.children) {
            Entity child(childId, m_scene);
            DrawEntityNode(child);
        }
        ImGui::TreePop();
    }
}

template <typename T, typename UIFunction>
static void drawComponent(const std::string &name, Entity entity,
                          UIFunction uiFunction, bool removeable = true) {
    const ImGuiTreeNodeFlags treeNodeFlags =
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
        ImGuiTreeNodeFlags_SpanAvailWidth |
        ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
    if (entity.HasComponent<T>()) {
        auto &component = entity.GetComponent<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        float lineHeight =
            GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();
        bool open = ImGui::TreeNodeEx((void *)typeid(T).hash_code(),
                                      treeNodeFlags, "%s", name.c_str());
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

        if (removeComponent) entity.RemoveComponent<T>();
    }
}

void ScenePanel::DrawComponents(Entity &entity) {
    if (entity.HasComponent<TagComponent>()) {
        auto &id = entity.GetComponent<IDComponent>().id;
        std::string idString = "Resouce Id: " + std::to_string(id);
        ImGui::TextUnformatted(idString.c_str());
    }
    if (entity.HasComponent<TagComponent>()) {
        auto &tag = entity.GetComponent<TagComponent>().tag;

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
            if (!m_selectedEntity.HasComponent<ModelComponent>())
                m_selectedEntity.AddComponent<ModelComponent>();
            else
                SD_CORE_WARN("This entity already has the Model Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Terrain")) {
            if (!m_selectedEntity.HasComponent<TerrainComponent>())
                m_selectedEntity.AddComponent<TerrainComponent>();
            else
                SD_CORE_WARN("This entity already has the Terrain Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Light")) {
            if (!m_selectedEntity.HasComponent<LightComponent>())
                m_selectedEntity.AddComponent<LightComponent>();
            else
                SD_CORE_WARN("This entity already has the Light Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Text")) {
            if (!m_selectedEntity.HasComponent<TextComponent>())
                m_selectedEntity.AddComponent<TextComponent>();
            else
                SD_CORE_WARN("This entity already has the Text Component!");
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopItemWidth();

    drawComponent<TransformComponent>(
        "Transform", entity,
        [&](TransformComponent &component) {
            ImGui::RadioButton("World", &m_gizmoMode, ImGuizmo::WORLD);
            ImGui::SameLine();
            ImGui::RadioButton("Local", &m_gizmoMode, ImGuizmo::LOCAL);

            ImGui::RadioButton("Translate", &m_gizmoType, ImGuizmo::TRANSLATE);
            ImGui::SameLine();
            ImGui::RadioButton("Rotate", &m_gizmoType, ImGuizmo::ROTATE);
            ImGui::SameLine();
            ImGui::RadioButton("Scale", &m_gizmoType, ImGuizmo::SCALE);
            glm::vec3 position = component.transform.GetWorldPosition();
            if (ImGui::DrawVec3Control("Translation", position)) {
                component.transform.SetWorldPosition(position);
            }

            glm::vec3 rotation = glm::degrees(
                glm::eulerAngles(component.transform.GetWorldRotation()));
            if (ImGui::DrawVec3Control("Rotation", rotation)) {
                component.transform.SetWorldRotation(glm::radians(rotation));
            }

            glm::vec3 scale = component.transform.GetWorldScale();
            if (ImGui::DrawVec3Control("Scale", scale, 1.0f)) {
                component.transform.SetWorldScale(scale);
            }
        },
        false);
    drawComponent<ModelComponent>("Model", entity, [&](ModelComponent &mc) {
        static bool fileDialogOpen = false;
        static ImFileDialogInfo fileDialogInfo;
        std::string path = fileDialogInfo.resultPath.string();
        ImGui::InputText("##Path", path.data(), path.size(),
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            fileDialogOpen = true;
            fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
            fileDialogInfo.title = "Open File";
            fileDialogInfo.fileName = "";
            fileDialogInfo.directoryPath = std::filesystem::current_path();
        }
        if (ImGui::FileDialog(&fileDialogOpen, &fileDialogInfo)) {
            mc.id = asset->loadAsset<Model>(fileDialogInfo.resultPath);
        }

        ImGui::ColorEdit3("Color", &mc.color[0]);
        ImVec2 size(64, 64);
        auto model = asset->Get<Model>(mc.id);
        if (model) {
            DrawMaterialsList(model->GetMaterials(), size,
                              &m_selectedMaterialIdMap[entity]);
        }
    });
    // drawComponent<TerrainComponent>(
    //     "Terrain", entity, [&](TerrainComponent &terrain) {
    //         int gridSize = terrain.terrain.getGridSize();
    //         int vertexCount = terrain.terrain.getVertexCount();
    //         if (ImGui::InputInt("Grid size:", &gridSize)) {
    //             terrain.terrain.setGridSize(std::max(gridSize, 1));
    //             terrain.terrain.generateMesh();
    //             renderer->getTerrainSystem()->updateTerrain(entity);
    //         }
    //         if (ImGui::InputInt("Vertex count:", &vertexCount)) {
    //             terrain.terrain.setVertexCount(std::max(vertexCount, 2));
    //             terrain.terrain.generateMesh();
    //             renderer->getTerrainSystem()->updateTerrain(entity);
    //         }
    //     });
    drawComponent<LightComponent>(
        "Light", entity, [&](LightComponent &lightComp) {
            Light &light = lightComp.light;
            glm::vec3 diffuse = light.GetDiffuse();
            if (ImGui::ColorEdit3("Diffuse", &diffuse[0])) {
                light.SetDiffuse(diffuse);
            }
            glm::vec3 ambient = light.GetAmbient();
            if (ImGui::ColorEdit3("Ambient", &ambient[0])) {
                light.SetAmbient(ambient);
            }
            glm::vec3 specular = light.GetSpecular();
            if (ImGui::ColorEdit3("Specular", &specular[0])) {
                light.SetSpecular(specular);
            }
            bool isDirectional = light.IsDirectional();
            if (ImGui::Checkbox("Directional", &isDirectional)) {
                light.SetDirectional(isDirectional);
            }
            ImGui::SameLine();
            bool isCastShadow = light.IsCastShadow();
            if (ImGui::Checkbox("Cast Shadow", &isCastShadow)) {
                light.SetCastShadow(isCastShadow);
            }
            if (!light.IsDirectional()) {
                float constant = light.GetConstant();
                if (ImGui::SliderFloat("Constant", &constant, 0.f, 1.0f)) {
                    light.SetConstant(constant);
                }
                float linear = light.GetLinear();
                if (ImGui::SliderFloat("Linear", &linear, 0.f, 1.0f)) {
                    light.SetLinear(linear);
                }
                float quadratic = light.GetQuadratic();
                if (ImGui::SliderFloat("Quadratic", &quadratic, 0.0002f,
                                       0.1f)) {
                    light.SetQuadratic(quadratic);
                }
                float cutOff = glm::degrees(light.GetCutOff());
                if (ImGui::SliderFloat("cutOff", &cutOff, 0.f, 89.f)) {
                    light.SetCutOff(glm::radians(cutOff));
                }
                float outerCutOff = glm::degrees(light.GetOuterCutOff());
                if (ImGui::SliderFloat("outerCutOff", &outerCutOff, 1.0f,
                                       90.0f)) {
                    light.SetOuterCutOff(glm::radians(outerCutOff));
                }
            }
        });
    drawComponent<TextComponent>("Text", entity, [&](TextComponent &textComp) {
        // TODO: Can ImGui support UTF-16?
        static bool fileDialogOpen = false;
        static ImFileDialogInfo fileDialogInfo;
        char buffer[256];
        std::string utf8Str = WstringToString(textComp.text);
        memset(buffer, 0, sizeof(buffer));
        std::strncpy(buffer, utf8Str.c_str(), utf8Str.size());
        std::string path = fileDialogInfo.resultPath.string();
        ImGui::Text("Font File:");
        ImGui::InputText("##Path", path.data(), path.size(),
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            fileDialogOpen = true;
            fileDialogInfo.type = ImGuiFileDialogType_OpenFile;
            fileDialogInfo.title = "Open File";
            fileDialogInfo.fileName = "";
            fileDialogInfo.directoryPath = std::filesystem::current_path();
        }
        if (ImGui::FileDialog(&fileDialogOpen, &fileDialogInfo)) {
            textComp.id = asset->loadAsset<Font>(fileDialogInfo.resultPath);
        }
        ImGui::Text("Text Content:");
        if (ImGui::InputText("##TextEdit", buffer, sizeof(buffer))) {
            textComp.text = StringToWstring(buffer);
        }
        ImGui::Text("Pixel Size");
        ImGui::InputInt("##PixelSize", &textComp.pixelSize);
        ImGui::Text("Color");
        ImGui::ColorEdit4("##TextColor", &textComp.color[0]);
    });
}

void ScenePanel::DrawMaterialsList(const std::vector<Material> &materials,
                                   const ImVec2 &size, int *selected) {
    int itemSize = materials.size();
    if (!itemSize) return;

    std::string item;
    item = "Material " + std::to_string(*selected);
    if (ImGui::BeginCombo("##Materials", item.c_str())) {
        for (int i = 0; i < itemSize; i++) {
            item = "Material " + std::to_string(i);
            const bool is_selected = (*selected == i);
            if (ImGui::Selectable(item.c_str(), is_selected)) *selected = i;

            // Set the initial focus when opening the combo (scrolling +
            // keyboard navigation focus)
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    float width = ImGui::GetWindowWidth();
    const auto &material = materials[*selected];
    for (const auto &[type, texture] : material.GetTextures()) {
        ImGui::TextUnformatted(GetMaterialName(type).c_str());
        ImGui::SameLine(width / 2);
        ImGui::DrawTexture(*texture, size);
    }
}

}  // namespace SD
