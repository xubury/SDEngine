#include "AnimationEditor.hpp"
#include "EditorEvent.hpp"
#include "ECS/Component.hpp"
#include "Resource/Resource.hpp"
#include "Locator/Locator.hpp"

namespace SD {

AnimationEditor::AnimationEditor(EventDispatcher *dispatcher)
    : m_dispatcher(dispatcher), m_anim_index(0)
{
    m_entity_handler = m_dispatcher->Register<EntitySelectEvent>(
        [this](const EntitySelectEvent &e) {
            this->m_selected_entity = {e.entity_id, e.scene};
        });
}

void AnimationEditor::ImGui()
{
    ImGui::Begin("Anmiation Editor");
    {
        auto &cache = Locator<TextureCache>::Value();
        // ImGui::DrawTextureAssetSelection(cache, &m_texture_id);
        if (cache.Contains(m_texture_id)) {
            auto texture = cache.Get(m_texture_id);
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
