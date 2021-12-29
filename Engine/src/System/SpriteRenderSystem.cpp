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
    renderer->End();

    // draw tile map
    device->Disable(SD::Operation::DEPTH_TEST);
    renderer->Begin(*scene->GetCamera());
    auto tilemap_comp = scene->view<TileMapComponent>();
    scene->sort<TileMapComponent>(
        [](const TileMapComponent &lhs, const TileMapComponent &rhs) {
            if (lhs.tiles.GetPriority() < rhs.tiles.GetPriority()) {
                return true;
            } else if (lhs.tiles.GetPriority() > rhs.tiles.GetPriority()) {
                return false;
            } else {
                return lhs.tiles.GetZ() < rhs.tiles.GetZ();
            }
        });
    tilemap_comp.each([this](const TileMapComponent &tilemap_comp) {
        auto &layout = tilemap_comp.tiles;
        if (!layout.GetVisible()) {
            return;
        }
        for (const auto &[pos, tile] : layout.GetTiles()) {
            renderer->DrawTexture(
                asset->Get<Sprite>(tile.sprite_id)->GetTexture(), tile.uvs,
                layout.MapTileToWorld(pos), glm::quat(1, 0, 0, 0),
                layout.GetTileSize());
        }
    });
    renderer->End();
    device->Enable(SD::Operation::DEPTH_TEST);
}

}  // namespace SD
