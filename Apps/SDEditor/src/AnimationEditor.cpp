#include "AnimationEditor.hpp"
#include "Asset/AssetStorage.hpp"

namespace SD {

template <typename T>
void SelectAsset(ResourceId *selected_id)
{
    auto &storage = AssetStorage::Get();

    if (!storage.Empty<T>()) {
        const Cache &cache = storage.GetCache<T>();
        std::string item = storage.Exists<T>(*selected_id)
                               ? storage.GetAsset<T>(*selected_id)->GetName()
                               : "NONE";
        if (ImGui::BeginCombo("##Assets", item.c_str())) {
            for (auto &[rid, asset] : cache) {
                item = asset->GetName();
                const bool is_selected = (*selected_id == asset->GetId());
                if (ImGui::Selectable(item.c_str(), is_selected)) {
                    *selected_id = asset->GetId();
                }

                // Set the initial focus when opening the combo (scrolling +
                // keyboard navigation focus)
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}

AnimationEditor::AnimationEditor() : System("Anmiation Editor"), m_anim_index(0)
{
}

void AnimationEditor::OnInit() { System::OnInit(); }

void AnimationEditor::OnPush()
{
    auto &dispatcher = GetEventDispatcher();
    m_entity_handler = dispatcher.Register<EntitySelectEvent>(
        [this](const EntitySelectEvent &e) {
            this->m_selected_entity = {e.entity_id, e.scene};
        });
}

void AnimationEditor::OnPop()
{
    auto &dispatcher = GetEventDispatcher();
    dispatcher.RemoveHandler(m_entity_handler);
}

void AnimationEditor::OnImGui()
{
    ImGui::Begin("Anmiation Editor");
    {
        SelectAsset<TextureAsset>(&m_texture_id);
        auto &storage = AssetStorage::Get();
        if (storage.Exists<TextureAsset>(m_texture_id)) {
            auto texture =
                storage.GetAsset<TextureAsset>(m_texture_id)->GetTexture();
            ImGui::DrawTileTexture(*texture, m_tile_size, m_uvs, &m_count);
            if (m_selected_entity &&
                m_selected_entity.HasComponent<SpriteAnimationComponent>()) {
                auto &anim_comp =
                    m_selected_entity.GetComponent<SpriteAnimationComponent>();
                int item_size = anim_comp.animations.size();
                if (item_size > 0) {
                    std::string item = "Anim " + std::to_string(m_anim_index);
                    if (ImGui::BeginCombo("##Animations", item.c_str())) {
                        for (int i = 0; i < item_size; ++i) {
                            item = "Anim: " + std::to_string(i);
                            const bool is_selected = (m_anim_index == i);
                            if (ImGui::Selectable(item.c_str(), is_selected))
                                m_anim_index = i;

                            // Set the initial focus when opening the combo
                            // (scrolling + keyboard navigation focus)
                            if (is_selected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Add Anim")) {
                    anim_comp.animations.push_back(
                        FrameAnimation<SpriteFrame>());
                }
                if (ImGui::Button("Add Frame")) {
                    anim_comp.animations[m_anim_index].PushBack(SpriteFrame{
                        m_texture_id, m_uvs, m_count * m_tile_size, 0});
                }
            }
        }
    }
    ImGui::End();
}

}  // namespace SD
