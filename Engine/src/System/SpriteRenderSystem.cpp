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

    device->SetFramebuffer(renderer->GetFramebuffer());
    renderer->Begin(*scene->GetCamera());
    textView.each([this](entt::entity entity_id,
                         const TransformComponent &transformComp,
                         const TextComponent &textComp) {
        renderer->SetTextOrigin(0, 0);
        auto font = asset->Get<Font>(textComp.id);
        if (font) {
            renderer->DrawText(*font, textComp.text,
                               transformComp.GetWorldTransform().GetMatrix(),
                               textComp.color,
                               static_cast<uint32_t>(entity_id));
        }
    });
    renderer->End();

    // draw tile map
    device->SetDepthMask(false);
    renderer->Begin(*scene->GetCamera());
    auto tilemap_comp = scene->view<SpriteComponent, TransformComponent>();
    scene->sort<SpriteComponent>(
        [this](const entt::entity lhs, const entt::entity rhs) {
            auto l_z = scene->get<TransformComponent>(lhs).GetWorldPosition().z;
            auto r_z = scene->get<TransformComponent>(rhs).GetWorldPosition().z;
            auto l_p = scene->get<SpriteComponent>(lhs).priority;
            auto r_p = scene->get<SpriteComponent>(rhs).priority;
            if (l_z < r_z) {
                return true;
            } else if (l_z > r_z) {
                return false;
            } else {
                return l_p < r_p;
            }
        });
    tilemap_comp.each([this](entt::entity entity_id,
                             const SpriteComponent &sprite_comp,
                             const TransformComponent &transform_comp) {
        uint32_t id = static_cast<uint32_t>(entity_id);
        renderer->DrawTexture(asset->Get<Sprite>(sprite_comp.id)->GetTexture(),
                              sprite_comp.uvs,
                              transform_comp.GetWorldPosition(),
                              transform_comp.GetWorldRotation(),
                              sprite_comp.size, glm::vec4(1.0f), id);
    });
    renderer->End();
    device->SetDepthMask(true);
}

}  // namespace SD
