#include "AnimationEditor.hpp"
#include "Renderer/Sprite.hpp"

namespace SD {

AnimationEditor::AnimationEditor()
    : System("Anmiation Editor"), m_anim_index(0) {}

void AnimationEditor::OnPush() {
    m_entity_handler = dispatcher->Register<EntitySelectEvent>(
        [this](const EntitySelectEvent &e) {
            this->m_selected_entity = {e.entity_id, e.scene};
        });
}

void AnimationEditor::OnPop() { dispatcher->RemoveHandler(m_entity_handler); }

void AnimationEditor::OnImGui() {
    ImGui::Begin("Anmiation Editor");
    {
        ImGui::InputText("##Path", m_sprite_path.data(), m_sprite_path.size(),
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            m_is_dialog_open = true;
            m_dialog_info.type = ImGuiFileDialogType::OPEN_FILE;
            m_dialog_info.title = "Open File";
            m_dialog_info.file_name = "";
            m_dialog_info.directory_path = asset->GetRootPath();
            m_dialog_info.regex_match = IMG_FILTER;
        }
        if (ImGui::FileDialog(&m_is_dialog_open, &m_dialog_info)) {
            m_sprite_id = asset->LoadAsset<Sprite>(m_dialog_info.result_path);
            m_sprite_path = asset->GetAssetPath(m_sprite_id);
        }
        auto sprite = asset->Get<Sprite>(m_sprite_id);
        if (sprite) {
            ImGui::DrawTileTexture(*sprite->GetTexture(), m_tile_size, m_uvs,
                                   &m_count);
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
                    anim_comp.animations[m_anim_index].PushBack(
                        SpriteFrame{m_sprite_id, m_sprite_path, m_uvs,
                                    m_count * m_tile_size});
                }
            }
        }
    }
    ImGui::End();
}

}  // namespace SD
