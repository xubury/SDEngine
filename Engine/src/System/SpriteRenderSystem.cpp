#include "System/SpriteRenderSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Font.hpp"
#include "Renderer/Bitmap.hpp"
#include "Renderer/Sprite.hpp"

namespace SD {

SpriteRenderSystem::SpriteRenderSystem() : System("SpriteRenderSystem") {}

void SpriteRenderSystem::OnPush() {}

void SpriteRenderSystem::OnPop() {}

void SpriteRenderSystem::OnRender() {
    auto textView = scene->view<TransformComponent, TextComponent>();

    device->SetTarget(renderer->GetDefaultTarget());
    renderer->Begin(*scene->GetCamera());
    textView.each([this](const TransformComponent &transformComp,
                         const TextComponent &textComp) {
        renderer->SetTextOrigin(0, 0);
        auto font = asset->Get<Font>(textComp.id);
        if (font) {
            renderer->DrawText(*font, textComp.text,
                               transformComp.transform.GetWorldTransform(),
                               textComp.color);
        }
    });

    // draw tile map
    auto tilemap_comp = scene->view<TileMapComponent>();
    tilemap_comp.each([this](const TileMapComponent &tilemap_comp) {
        auto &layout = tilemap_comp.tiles;
        for (const auto &[pos, tile] : layout.GetTiles()) {
            renderer->DrawTexture(
                asset->Get<Sprite>(tile.sprite_id)->GetTexture(), tile.uvs,
                glm::vec3(layout.MapTileToWorld(pos), 0), glm::quat(1, 0, 0, 0),
                layout.GetTileSize());
        }
    });
    renderer->End();
}

}  // namespace SD
