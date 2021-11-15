#include "Renderer/System/SpriteRenderSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"

namespace SD {

SpriteRenderSystem::SpriteRenderSystem(RenderTarget *target)
    : System("SpriteRender"), m_target(target) {}

void SpriteRenderSystem::onPush() {}

void SpriteRenderSystem::onPop() {}

void SpriteRenderSystem::onRender() {
    auto scene = renderer->getScene();
    auto textView = scene->view<TransformComponent, TextComponent>();

    renderer->setRenderTarget(*m_target);
    renderer->beginScene(*renderer->getCamera());
    textView.each([this](const TransformComponent &transformComp,
                         const TextComponent &textComp) {
        renderer->setTextOrigin(0, 0);
        auto font = AssetManager::instance().get<Font>(textComp.id);
        if (font) {
            renderer->drawText(*font, textComp.text, textComp.pixelSize,
                               transformComp.transform.getWorldTransform(),
                               textComp.color);
        }
    });
    renderer->endScene();
}

}  // namespace SD
