#include "ScenePanel.hpp"
#include "EditorEvent.hpp"
#include "Utility/Log.hpp"
#include "Utility/String.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Resource/Resource.hpp"
#include "Locator/Locator.hpp"

#define ECS_MOVEENTITY "ECS MOVEENTITY"

namespace SD {

ScenePanel::ScenePanel(EventDispatcher *dispatcher)
    : m_dispatcher(dispatcher),
      m_gizmo_mode(ImGuizmo::WORLD),
      m_gizmo_op(ImGuizmo::TRANSLATE)
{
    m_entity_select_handler = dispatcher->Register<EntitySelectEvent>(
        [this](const EntitySelectEvent &e) {
            this->m_selected_entity = {e.entity_id, e.scene};
        });
}

void ScenePanel::Reset()
{
    m_selected_material_id_map.clear();
    m_selected_anim_id_map.clear();
}

ImGuizmo::MODE ScenePanel::GetGizmoMode() const { return m_gizmo_mode; }

ImGuizmo::OPERATION ScenePanel::GetGizmoOperation() const { return m_gizmo_op; }

void ScenePanel::ImGui(Scene *scene)
{
    ImGui::Begin("Scene Hierarchy");

    auto &entities = scene->GetEntityRegistry();
    entities.each([&](auto entityID) {
        Entity entity{entityID, scene};

        TransformComponent &data = entity.GetComponent<TransformComponent>();
        Entity parent(data.parent, scene);
        if (!parent) {
            DrawEntityNode(entity);
        }
    });

    if (m_entity_to_destroy) {
        if (m_selected_entity == m_entity_to_destroy)
            m_dispatcher->PublishEvent(EntitySelectEvent());
        m_entity_to_destroy.Destroy();
        m_entity_to_destroy = {};
    }

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        m_dispatcher->PublishEvent(EntitySelectEvent());

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
                          scene);
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

void ScenePanel::DrawEntityNode(Entity &entity)
{
    auto &data = entity.GetComponent<TransformComponent>();
    auto &tag = entity.GetComponent<TagComponent>().tag;

    static ImGuiTreeNodeFlags base_flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_SpanAvailWidth;
    static ImGuiTreeNodeFlags leaf_flags =
        ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth;

    ImGuiTreeNodeFlags flags = data.children.empty() ? leaf_flags : base_flags;
    flags |= ((m_selected_entity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
             ImGuiTreeNodeFlags_OpenOnArrow;
    bool opened = ImGui::TreeNodeEx((void *)(uint64_t)(entt::entity)entity,
                                    flags, "%s", tag.c_str());
    if (ImGui::IsItemClicked(0)) {
        m_dispatcher->PublishEvent(
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
        m_dispatcher->PublishEvent(
            EntitySelectEvent{m_selected_entity, m_selected_entity.GetScene()});
        if (ImGui::MenuItem("Delete Entity")) {
            m_entity_to_destroy = entity;
        }
        if (ImGui::MenuItem("Create Empty Entity")) {
            Entity child = entity.GetScene()->CreateEntity("Empty Entity");
            entity.AddChild(child);
        }
        if (ImGui::MenuItem("Clone Entity")) {
            entity.GetScene()->CloneEntity(entity);
        }

        ImGui::EndPopup();
    }

    if (opened) {
        for (entt::entity childId : data.children) {
            Entity child(childId, entity.GetScene());
            DrawEntityNode(child);
        }
        ImGui::TreePop();
    }
}

template <typename T, typename UIFunction>
static void DrawComponent(const std::string &name, Entity entity,
                          UIFunction uiFunction, bool removeable = true)
{
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

void ScenePanel::DrawComponents(Entity &entity)
{
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
        if (ImGui::MenuItem("Directional Light")) {
            if (!entity.HasComponent<DirectionalLightComponent>())
                entity.AddComponent<DirectionalLightComponent>();
            else
                SD_CORE_WARN("This entity already has the Light Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Point Light")) {
            if (!entity.HasComponent<PointLightComponent>())
                entity.AddComponent<PointLightComponent>();
            else
                SD_CORE_WARN(
                    "This entity already has the Point Light Component!");
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
                entity.AddComponent<CameraComponent>(CameraType::Perspective,
                                                     glm::radians(45.f), 400,
                                                     400, 0.1f, 1000.f);
            else
                SD_CORE_WARN("This entity already has the Camera Component!");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Sprite")) {
            if (!entity.HasComponent<SpriteComponent>())
                entity.AddComponent<SpriteComponent>();
            else
                SD_CORE_WARN("This entity already has the Sprite Component!");
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

    auto &texture_cache = Locator<TextureCache>::Value();
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
            bool in_world = m_gizmo_mode == ImGuizmo::WORLD;
            Vector3f position = in_world ? component.GetWorldPosition()
                                         : component.GetLocalPosition();
            if (ImGui::DrawVec3Control("Translation", position)) {
                in_world ? component.SetWorldPosition(position)
                         : component.SetLocalPosition(position);
            }

            Vector3f rotation = glm::degrees(
                glm::eulerAngles(in_world ? component.GetWorldRotation()
                                          : component.GetLocalRotation()));
            if (ImGui::DrawVec3Control("Rotation", rotation)) {
                in_world ? component.SetWorldRotation(glm::radians(rotation))
                         : component.SetLocalRotation(glm::radians(rotation));
            }

            Vector3f scale = in_world ? component.GetWorldScale()
                                      : component.GetLocalScale();
            if (ImGui::DrawVec3Control("Scale", scale, 1.0f)) {
                in_world ? component.SetWorldScale(scale)
                         : component.SetLocalScale(scale);
            }
        },
        false);
    DrawComponent<MeshComponent>(
        "Mesh", entity, [&](MeshComponent &mc) { DrawMaterial(mc.material); });
    DrawComponent<DirectionalLightComponent>(
        "Directional Light", entity, [&](DirectionalLightComponent &lightComp) {
            DirectionalLight &light = lightComp.light;
            CascadeShadow &shadow = lightComp.shadow;
            ImGui::ColorEdit3("Diffuse", &light.diffuse[0]);
            ImGui::ColorEdit3("Ambient", &light.ambient[0]);
            ImGui::ColorEdit3("Specular", &light.specular[0]);
            if (ImGui::Checkbox("Cast Shadow", &lightComp.is_cast_shadow)) {
                if (lightComp.is_cast_shadow) {
                    lightComp.shadow.CreateShadowMap();
                }
                else {
                    lightComp.shadow.DestroyShadowMap();
                }
            }
            auto planes = shadow.GetCascadePlanes();
            int num_of_cascades = planes.size();
            if (ImGui::SliderInt("Num of Cascades", &num_of_cascades, 1, 4)) {
                shadow.SetNumOfCascades(num_of_cascades);
            }
            if (ImGui::InputFloat4("Cascades", planes.data())) {
                shadow.SetCascadePlanes(planes);
            }
        });
    DrawComponent<PointLightComponent>(
        "Point Light", entity, [&](PointLightComponent &lightComp) {
            PointLight &light = lightComp.light;
            ImGui::ColorEdit3("Diffuse", &light.diffuse[0]);

            ImGui::ColorEdit3("Ambient", &light.ambient[0]);

            ImGui::ColorEdit3("Specular", &light.specular[0]);

            ImGui::SliderFloat("Constant", &light.constant, 0.f, 1.0f);
            ImGui::SliderFloat("Linear", &light.linear, 0.f, 1.0f);
            ImGui::SliderFloat("Quadratic", &light.quadratic, 0.0002f, 0.1f,
                               "%.4f");

            if (ImGui::Checkbox("Cast Shadow", &lightComp.is_cast_shadow)) {
                if (lightComp.is_cast_shadow) {
                    lightComp.shadow.CreateShadowMap();
                }
                else {
                    lightComp.shadow.DestroyShadowMap();
                }
            }
            float far_z = lightComp.shadow.GetFarZ();
            if (ImGui::SliderFloat("Shadow Far Z", &far_z, 1.0f, 500.f)) {
                lightComp.shadow.SetFarZ(far_z);
            }
        });
    DrawComponent<TextComponent>("Text", entity, [&](TextComponent &textComp) {
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
            using underlying = std::underlying_type<CameraType>::type;
            CameraType type = cameraComp.camera.GetCameraType();
            ImGui::Checkbox("Primary", &cameraComp.primary);
            if (ImGui::RadioButton(
                    "Perspective", reinterpret_cast<underlying *>(&type),
                    static_cast<underlying>(CameraType::Perspective))) {
                cameraComp.camera.SetCameraType(type);
            }
            ImGui::SameLine();
            if (ImGui::RadioButton(
                    "Ortho", reinterpret_cast<underlying *>(&type),
                    static_cast<underlying>(CameraType::Orthographic))) {
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
    DrawComponent<SpriteComponent>(
        "Sprite", entity, [&](SpriteComponent &sprite_comp) {
            auto &frame = sprite_comp.frame;
            // ImGui::DrawTextureAssetSelection(texture_cache,
            // &frame.texture_id);
            ImGui::TextUnformatted("Size");
            ImGui::InputFloat2("##Size", &frame.size[0]);
            ImGui::TextUnformatted("Prioirty");
            ImGui::InputInt("##Priority", &frame.priority);
            if (texture_cache.Contains(frame.texture_id)) {
                auto texture = texture_cache.Handle(frame.texture_id);
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
                if (texture_cache.Contains(frame.texture_id)) {
                    auto texture = texture_cache.Handle(frame.texture_id);
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
                }
                else {
                    anim_comp.animator.Play(&anim);
                }
            }
        });
}

void ScenePanel::DrawMaterial(Material &material)
{
    float width = ImGui::GetWindowWidth();
    for (int i = static_cast<int>(MaterialType::None) + 1;
         i < static_cast<int>(MaterialType::Shininess) + 1; ++i) {
        MaterialType type = static_cast<MaterialType>(i);
        const Texture *texture = material.GetTexture(type);
        if (texture) {
            ImGui::TextUnformatted(GetMaterialName(type).c_str());
            ImGui::SameLine(width / 2);
            ImGui::DrawTexture(*texture);
        }
    }

    Vector3f diffuse_color = material.GetDiffuseColor();
    Vector3f ambient_color = material.GetAmbientColor();
    Vector3f emissive_color = material.GetEmissiveColor();
    if (ImGui::ColorEdit3("Diffuse Color", &diffuse_color[0])) {
        material.SetDiffuseColor(diffuse_color);
    }
    if (ImGui::ColorEdit3("Ambient Color", &ambient_color[0])) {
        material.SetAmbientColor(ambient_color);
    }
    if (ImGui::ColorEdit3("Emssive Color", &emissive_color[0])) {
        material.SetEmissiveColor(emissive_color);
    }
}

void ScenePanel::DrawAnimList(
    const std::vector<FrameAnimation<SpriteFrame>> &anims, int *selected)
{
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
