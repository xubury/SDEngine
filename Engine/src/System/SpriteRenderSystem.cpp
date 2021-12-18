#include "System/SpriteRenderSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Font.hpp"
#include "Renderer/Bitmap.hpp"

namespace SD {

SpriteRenderSystem::SpriteRenderSystem()
    : System("SpriteRenderSystem") {}

void SpriteRenderSystem::OnPush() {}

void SpriteRenderSystem::OnPop() {}

void SpriteRenderSystem::OnRender() {
    auto textView = scene->view<TransformComponent, TextComponent>();

    renderer->GetDefaultTarget().Bind();
    renderer->Begin(*scene->GetCamera());
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
    renderer->End();
}

}  // namespace SD
