#ifndef SD_ANMIATION_EDITOR_HPP
#define SD_ANMIATION_EDITOR_HPP

#include "ImGui/FileDialog.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "Resource/Resource.hpp"

#include "ECS/Entity.hpp"
#include "Utility/ResourceId.hpp"
#include "Utility/EventDispatcher.hpp"

namespace SD {

class AnimationEditor {
   public:
    AnimationEditor(EventDispatcher *disaptcher);
    void ImGui(TextureCache &cache);

   private:
    EventDispatcher *m_dispatcher;
    HandlerRegistration m_entity_handler;
    Entity m_selected_entity;

    ImFileDialogInfo m_dialog_info;
    bool m_is_dialog_open;

    ResourceId m_texture_id;
    std::array<Vector2f, 2> m_uvs;
    Vector2i m_count;
    Vector2i m_tile_size;

    int m_anim_index;
};

}  // namespace SD

#endif
