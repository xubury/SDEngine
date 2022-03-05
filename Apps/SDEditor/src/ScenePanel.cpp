#include "ScenePanel.hpp"
#include "Utility/Log.hpp"
#include "Utility/String.hpp"
#include "ImGui/ImGuiWidget.hpp"

#include "Asset/AssetStorage.hpp"
#include "Asset/TextureAsset.hpp"
#include "Asset/FontAsset.hpp"
#include "Asset/ModelAsset.hpp"

#define ECS_MOVEENTITY "ECS MOVEENTITY"

namespace SD {

template <typename T>
void SelectAsset(ResourceId *selected_id) {
    auto &storage = AssetStorage::Get();

    if (!storage.Empty<T>()) {
        const Cache &cache = storage.GetCache<T>();
        std::string item = storage.Exists<T>(*selected_id)
                               ? storage.GetAsset<T>(*selected_id)->GetPath()
                               : "NONE";
        if (ImGui::BeginCombo("##Assets", item.c_str())) {
            for (auto &[rid, asset] : cache) {
                item = asset->GetPath();
                const bool is_selected = (*selected_id == asset->GetId());
                if (ImGui::Selectable(item.c_str(), is_selected)) {
                }
                *selected_id = asset->GetId();

                // Set the initial focus when opening the combo (scrolling +
                // keyboard navigation focus)
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}

ScenePanel::ScenePanel()
    : System("ScenePanel"),
      m_gizmo_mode(ImGuizmo::WORLD),
      m_gizmo_op(ImGuizmo::TRANSLATE) {}

void ScenePanel::OnPush() {
    m_size_handler =
        EventSystem::Get().Register(this, &ScenePanel::OnSizeEvent);
    m_entity_select_handler = EventSystem::Get().Register<EntitySelectEvent>(
        [this](const EntitySelectEvent &e) {
            this->m_selected_entity = {e.entity_id, e.scene};
        });
}

void ScenePanel::OnPop() {
    EventSystem::Get().RemoveHandler(m_size_handler);
    EventSystem::Get().RemoveHandler(m_entity_select_handler);
}

void ScenePanel::OnSizeEvent(const ViewportSizeEvent &event) {
    m_width = event.width;
    m_height = event.height;
}

void ScenePanel::Reset() {
    m_selected_material_id_map.clear();
    m_selected_anim_id_map.clear();
}

ImGuizmo::MODE ScenePanel::GetGizmoMode() const { return m_gizmo_mode; }

ImGuizmo::OPERATION ScenePanel::GetGizmoOperation() const { return m_gizmo_op; }

void ScenePanel::OnImGui() {
    ImGui::Begin("Scene Hierarchy");

    scene->each([&](auto entityID) {
        Entity entity{entityID, scene.get()};

        TransformComponent &data = entity.GetComponent<TransformComponent>();
        Entity parent(data.parent, scene.get());
        if (!parent) {
            DrawEntityNode(entity);
        }
    });

    if (m_entity_to_destroy) {
        if (m_selected_entity == m_entity_to_destroy)
            EventSystem::Get().PublishEvent(EntitySelectEvent());
        m_entity_to_destroy.Destroy();
        m_entity_to_destroy = {};
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        EventSystem::Get().PublishEvent(EntitySelectEvent());

    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(0, 1, false)) {
        if (ImGui::MenuItem("Create Empty Entity"))
            scene->CreateEntity("Empty Entity");

        ImGui::EndPopup();
    }
    ImGuiID id = ImGui::GetWindowDockID();
    ImVec2 min = ImVec2(ImGui::GetWindowPos().x + ImGui::GetCursorPosX(),
                        ImGui::GetWindowPos().y + ImGui::GetCursorPosY());
    ImVec2 max = ImVec2(min.x + ImGui::GetWindowSize().x,
                        min.y + ImGui::GetWindowSize().y);

    if (ImGui::BeginDragDropTargetCustom(ImRect(min, max), id)) {
        if (const ImGuiPayload *payload =
                ImGui::AcceptDragDropPayload(ECS_MOVEENTITY)) {
            IM_ASSERT(payload->DataSize == sizeof(Entity));
            Entity entity = *(Entity *)payload->Data;

            Entity parent(entity.GetComponent<TransformComponent>().parent,
                          scene.get());
            if (parent) {
                parent.RemoveChild(entity);
            }
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::End();

    ImGui::Begin("Properties");
    if (m_selected_entity) {
        DrawComponents(m_selected_entity);
    }

    ImGui::End();
}

void ScenePanel::DrawEntityNode(Entity &entity) {
    auto &data = entity.GetComponent<TransformComponent>();

    auto &tag = entity.GetComponent<TagComponent>().tag;

    static ImGuiTreeNodeFlags base_flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
    static ImGuiTreeNodeFlags leaf_flags =
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags flags = data.children.empty() ? leaf_flags : base_flags;
    flags |= ((m_selected_entity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
             ImGuiTreeNodeFlags_OpenOnArrow;
    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(entt::entity)entity,
                                    flags, "%s", tag.c_str());
    if (ImGui::IsItemClicked(0)) {
        EventSystem::Get().PublishEvent(
            EntitySelectEvent{entity, entity.GetScene()});
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetDragDropPayload(ECS_MOVEENTITY, &entity, sizeof(Entity));

        // Display preview
        ImGui::TextUnformatted(entity.GetComponent<TagComponent>().tag.c_str());
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload =
                ImGui::AcceptDragDropPayload(ECS_MOVEENTITY)) {
            IM_ASSERT(payload->DataSize == sizeof(Entity));
            entity.AddChild(*(Entity *)payload->Data);
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginPopupContextItem()) {
        EventSystem::Get().PublishEvent(
            EntitySelectEvent{m_selected_entity, m_selected_entity.GetScene()});
        if (ImGui::MenuItem("Delete Entity")) {
            m_entity_to_destroy = entity;
        }
        if (ImGui::MenuItem("Create Empty Entity")) {
            Entity child = scene->CreateEntity("Empty Entity");
            entity.AddChild(child);
        }
        if (ImGui::MenuItem("Clone Entity")) {
            scene->CloneEntity(entity);
        }

        ImGui::EndPopup();
    }

    if (opened) {
        for (entt::entity childId : data.children) {
            Entity child(childId, scene.get());
            DrawEntityNode(child);
        }
        ImGui::TreePop();
    }
}

template <typename T, typename UIFunction>
static void DrawComponent(const std::string &name, Entity entity,
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
        auto &id = entity.GetComponent<IdComponent>().id;
        std::string idString = "Resource Id: " + std::to_string(id);
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
            if (!entity.HasComponent<ModelComponent>())
                entity.AddComponent<ModelComponent>();
            else
                SD_CORE_WARN("This entity already has the Model Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Light")) {
            if (!entity.HasComponent<LightComponent>())
                entity.AddComponent<LightComponent>();
            else
                SD_CORE_WARN("This entity already has the Light Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Text")) {
            if (!entity.HasComponent<TextComponent>())
                entity.AddComponent<TextComponent>();
            else
                SD_CORE_WARN("This entity already has the Text Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Camera")) {
            if (!entity.HasComponent<CameraComponent>())
                entity.AddComponent<CameraComponent>(
                    CameraType::PERSPECTIVE, glm::radians(45.f), m_width,
                    m_height, 0.1f, 1000.f);
            else
                SD_CORE_WARN("This entity already has the Camera Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Sprite Animation")) {
            if (!entity.HasComponent<SpriteAnimationComponent>())
                entity.AddComponent<SpriteAnimationComponent>();
            else
                SD_CORE_WARN(
                    "This entity already has the Sprite Animation Component!");
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopItemWidth();

    DrawComponent<TransformComponent>(
        "Transform", entity,
        [&](TransformComponent &component) {
            ImGui::RadioButton("World", reinterpret_cast<int *>(&m_gizmo_mode),
                               ImGuizmo::WORLD);
            ImGui::SameLine();
            ImGui::RadioButton("Local", reinterpret_cast<int *>(&m_gizmo_mode),
                               ImGuizmo::LOCAL);

            ImGui::RadioButton("Translate",
                               reinterpret_cast<int *>(&m_gizmo_op),
                               ImGuizmo::TRANSLATE);
            ImGui::SameLine();
            ImGui::RadioButton("Rotate", reinterpret_cast<int *>(&m_gizmo_op),
                               ImGuizmo::ROTATE);
            ImGui::SameLine();
            ImGui::RadioButton("Scale", reinterpret_cast<int *>(&m_gizmo_op),
                               ImGuizmo::SCALE);
            glm::vec3 position = component.GetWorldPosition();
            if (ImGui::DrawVec3Control("Translation", position)) {
                component.SetWorldPosition(position);
            }

            glm::vec3 rotation =
                glm::degrees(glm::eulerAngles(component.GetWorldRotation()));
            if (ImGui::DrawVec3Control("Rotation", rotation)) {
                component.SetWorldRotation(glm::radians(rotation));
            }

            glm::vec3 scale = component.GetWorldScale();
            if (ImGui::DrawVec3Control("Scale", scale, 1.0f)) {
                component.SetWorldScale(scale);
            }
        },
        false);
    DrawComponent<ModelComponent>("Model", entity, [&](ModelComponent &mc) {
        SelectAsset<ModelAsset>(&mc.model_id);
        // if (AssetStorage::Get().Exists<model_path>(mc.model_id)) {
        //     ModelAsset *model_asset =
        //         AssetStorage::Get().GetAsset<ModelAsset>(mc.model_id);
        //     mc.model_path = model_asset->GetPath();
        // }

        // ImGui::ColorEdit3("Color", &mc.color[0]);
        // ResourceId rid(fileDialogInfo.result_path.string());
        // if (AssetStorage::Get().Exists<ModelAsset>(rid)) {
        //     auto model = AssetStorage::Get()
        //                      .GetAsset<ModelAsset>(ResourceId(rid))
        //                      ->GetModel();
        //     DrawMaterialsList(model->GetMaterials(),
        //                       &m_selected_material_id_map[entity]);
        // }
    });
    DrawComponent<LightComponent>(
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
                float cutOff = glm::degrees(light.GetCutoff());
                if (ImGui::SliderFloat("cutOff", &cutOff, 0.f, 89.f)) {
                    light.SetCutoff(glm::radians(cutOff));
                }
                float outerCutOff = glm::degrees(light.GetOuterCutoff());
                if (ImGui::SliderFloat("outerCutOff", &outerCutOff, 1.0f,
                                       90.0f)) {
                    light.SetOuterCutoff(glm::radians(outerCutOff));
                }
            }
        });
    DrawComponent<TextComponent>("Text", entity, [&](TextComponent &textComp) {
        static bool fileDialogOpen = false;
        static ImFileDialogInfo fileDialogInfo;
        ImGui::Text("Font File:");
        ImGui::InputText("##Path", textComp.font_path.data(),
                         textComp.font_path.size(),
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            fileDialogOpen = true;
            fileDialogInfo.type = ImGuiFileDialogType::OPEN_FILE;
            fileDialogInfo.title = "Open File";
            fileDialogInfo.regex_match = FONT_FILTER;
            fileDialogInfo.file_name = "";
            fileDialogInfo.directory_path = AssetStorage::Get().GetDirectory();
        }
        if (ImGui::FileDialog(&fileDialogOpen, &fileDialogInfo)) {
            try {
                auto font_asset = AssetStorage::Get().LoadAsset<FontAsset>(
                    fileDialogInfo.result_path.string());
                textComp.font_id = font_asset->GetId();
                textComp.font_path = font_asset->GetPath();
            } catch (const Exception &e) {
                SD_CORE_ERROR("Error loading font: {}", e.what());
            }
        }
        ImGui::Text("Text Content:");
        static char buffer[256];
        std::copy(textComp.text.begin(), textComp.text.end(), buffer);
        if (ImGui::InputText("##TextEdit", buffer, sizeof(buffer))) {
            textComp.text = buffer;
        }
        ImGui::Text("Color");
        ImGui::ColorEdit4("##TextColor", &textComp.color[0]);
    });
    DrawComponent<CameraComponent>(
        "Camera", entity, [&](CameraComponent &cameraComp) {
            CameraType type = cameraComp.camera.GetCameraType();
            if (ImGui::RadioButton("Perspective",
                                   reinterpret_cast<int *>(&type),
                                   static_cast<int>(CameraType::PERSPECTIVE))) {
                cameraComp.camera.SetCameraType(type);
            }
            ImGui::SameLine();
            if (ImGui::RadioButton(
                    "Ortho", reinterpret_cast<int *>(&type),
                    static_cast<int>(CameraType::ORTHOGRAPHIC))) {
                cameraComp.camera.SetCameraType(type);
            }
            float fov = cameraComp.camera.GetFOV();
            ImGui::Text("Field of view");
            if (ImGui::SliderAngle("##FOV", &fov, 1.0f, 89.f)) {
                cameraComp.camera.SetFOV(fov);
            }
            float near_z = cameraComp.camera.GetNearZ();
            float far_z = cameraComp.camera.GetFarZ();
            ImGui::Text("Near Z");
            if (ImGui::SliderFloat("##Near Z", &near_z, 1e-3, far_z)) {
                cameraComp.camera.SetNearZ(near_z);
            }
            ImGui::Text("Far Z");
            if (ImGui::SliderFloat("##Far Z", &far_z, near_z, 1000)) {
                cameraComp.camera.SetFarZ(far_z);
            }
        });
    DrawComponent<PriorityComponent>(
        "Priority", entity, [&](PriorityComponent &priority_comp) {
            ImGui::TextUnformatted("Prioirty");
            ImGui::InputInt("##Priority", &priority_comp.priority);
        });
    DrawComponent<SpriteComponent>(
        "Sprite", entity, [&](SpriteComponent &sprite_comp) {
            auto &frame = sprite_comp.frame;
            if (AssetStorage::Get().Exists<TextureAsset>(frame.texture_id)) {
                auto texture = AssetStorage::Get()
                                   .GetAsset<TextureAsset>(frame.texture_id)
                                   ->GetTexture();
                ImGui::DrawTexture(*texture,
                                   ImVec2(frame.uvs[0].x, frame.uvs[0].y),
                                   ImVec2(frame.uvs[1].x, frame.uvs[1].y));
            }
        });
    DrawComponent<SpriteAnimationComponent>(
        "Sprite Animation", entity, [&](SpriteAnimationComponent &anim_comp) {
            DrawAnimList(anim_comp.animations, &m_selected_anim_id_map[entity]);
            auto &anim =
                anim_comp.animations.at(m_selected_anim_id_map[entity]);
            static int frame_index = 0;
            if (anim.GetFrameSize()) {
                ImGui::TextUnformatted("Frame:");
                ImGui::SliderInt("##Frame", &frame_index, 0,
                                 anim.GetFrameSize() - 1);
            }
            float speed = anim.GetSpeed();
            ImGui::TextUnformatted("FPS:");
            if (ImGui::InputFloat("##Anim Speed", &speed)) {
                anim.SetSpeed(speed);
            }
            if (frame_index < static_cast<int>(anim.GetFrameSize())) {
                const auto &frame = anim.GetFrame(frame_index);
                if (AssetStorage::Get().Exists<TextureAsset>(
                        frame.texture_id)) {
                    auto texture = AssetStorage::Get()
                                       .GetAsset<TextureAsset>(frame.texture_id)
                                       ->GetTexture();
                    ImGui::DrawTexture(*texture,
                                       ImVec2(frame.uvs[0].x, frame.uvs[0].y),
                                       ImVec2(frame.uvs[1].x, frame.uvs[1].y));
                }
            }
            bool loop = anim.IsLoop();
            if (ImGui::Checkbox("Loop", &loop)) {
                anim.SetLoop(loop);
            }
            bool is_playing = anim_comp.animator.IsPlaying();
            std::string button_str = is_playing ? "Stop" : "Play";
            if (ImGui::Button(button_str.c_str())) {
                if (is_playing) {
                    anim_comp.animator.Stop();
                } else {
                    anim_comp.animator.Play(&anim);
                }
            }
        });
}

void ScenePanel::DrawMaterialsList(const std::vector<Material> &materials,
                                   int *selected) {
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
        ImGui::DrawTexture(*texture);
    }
}

void ScenePanel::DrawAnimList(
    const std::vector<FrameAnimation<SpriteFrame>> &anims, int *selected) {
    int itemSize = anims.size();
    if (!itemSize) return;

    std::string item;
    item = "Anim " + std::to_string(*selected);
    if (ImGui::BeginCombo("##Animations", item.c_str())) {
        for (int i = 0; i < itemSize; i++) {
            item = "Anim " + std::to_string(i);
            const bool is_selected = (*selected == i);
            if (ImGui::Selectable(item.c_str(), is_selected)) *selected = i;

            // Set the initial focus when opening the combo (scrolling +
            // keyboard navigation focus)
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

}  // namespace SD
