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
    auto tilemap_comp = scene->view<TileMapComponent, TransformComponent>();
    scene->sort<TileMapComponent>([this](const entt::entity lhs,
                                         const entt::entity rhs) {
        auto l_z =
            scene->get<TransformComponent>(lhs).transform.GetWorldPosition().z;
        auto r_z =
            scene->get<TransformComponent>(rhs).transform.GetWorldPosition().z;
        auto l_p = scene->get<TileMapComponent>(lhs).tiles.GetPriority();
        auto r_p = scene->get<TileMapComponent>(rhs).tiles.GetPriority();
        if (l_z < r_z) {
            return true;
        } else if (l_z > r_z) {
            return false;
        } else {
            return l_p < r_p;
        }
    });
    tilemap_comp.each([this](const TileMapComponent &tilemap_comp,
                             const TransformComponent &transform_comp) {
        auto &layout = tilemap_comp.tiles;
        if (!layout.GetVisible()) {
            return;
        }
        for (const auto &[pos, tile] : layout.GetTiles()) {
            renderer->DrawTexture(
                asset->Get<Sprite>(tile.sprite_id)->GetTexture(), tile.uvs,
                layout.MapTileToWorld(pos, &transform_comp.transform),
                transform_comp.transform.GetWorldRotation(),
                layout.GetTileSize());
        }
    });
    renderer->End();
    device->Enable(SD::Operation::DEPTH_TEST);
}

}  // namespace SD
