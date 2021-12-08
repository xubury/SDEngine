#include "System/SpriteRenderSystem.hpp"
#include "ECS/Scene.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "Asset/Bitmap.hpp"

namespace SD {

SpriteRenderSystem::SpriteRenderSystem()
    : System("SpriteRenderSystem") {}

void SpriteRenderSystem::OnPush() {}

void SpriteRenderSystem::OnPop() {}

void SpriteRenderSystem::OnRender() {
    auto scene = renderer->GetScene();
    auto textView = scene->view<TransformComponent, TextComponent>();

    renderer->SetRenderTarget(renderer->GetDefaultTarget());
    renderer->BeginScene(*renderer->GetCamera());
    textView.each([this](const TransformComponent &transformComp,
                         const TextComponent &textComp) {
        renderer->SetTextOrigin(0, 0);
        auto font = asset->Get<Font>(textComp.id);
        if (font) {
            renderer->DrawText(*font, textComp.text, textComp.pixel_size,
                               transformComp.transform.GetWorldTransform(),
                               textComp.color);
        }
    });
    renderer->EndScene();
}

}  // namespace SD
