#ifndef SD_TILE_MAP_SYSTEM_HPP
#define SD_TILE_MAP_SYSTEM_HPP

#include "Core/System.hpp"
#include "TileMap/Export.hpp"
#include "TileMap/TileBrush.hpp"
#include "Renderer/Sprite.hpp"
#include "ImGui/ImGuiWidget.hpp"
#include "ImGui/FileDialog.hpp"

namespace SD {

class SD_TILE_MAP_API TileMapSystem : public System {
    enum Operation { NONE, ADD_ENTITY, REMOVE_ENTITY };

   public:
    TileMapSystem();

    void OnTick(float dt) override;

    void OnInit() override;
    void OnPush() override;
    void OnPop() override;

    void OnImGui() override;
    void OnRender() override;

   private:
    Ref<Texture> m_outline_texture;

    TileBrush m_brush;

    bool m_file_dialog_open;
    ImFileDialogInfo m_fileDialogInfo;

    // Selected sprite and its uvs
    ResourceId m_sprite_id;
    std::string m_sprite_path;
    std::array<glm::vec2, 2> m_uvs{glm::vec2{0, 0}, glm::vec2{0, 0}};
    int m_priority;

    bool m_draw_outline;

    Operation m_operation;

    HandlerRegistration m_entity_select_handler;
};

}  // namespace SD

#endif  // !SD_TILE_MAP_SYSTEM_HPP
