#include "Renderer/System/SpriteRenderSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/RenderEngine.hpp"
#include "Graphics/Device.hpp"

namespace sd {

SpriteRenderSystem::SpriteRenderSystem() {}

void SpriteRenderSystem::onInit() {}

void SpriteRenderSystem::onDestroy() {}

void SpriteRenderSystem::onRender() {
    auto scene = RenderEngine::getScene();
    auto textView = scene->view<TransformComponent, TextComponent>();

    RenderEngine::getRenderTarget().bind();
    Renderer::beginScene(*RenderEngine::getCamera());
    textView.each([](const TransformComponent &transformComp,
                     const TextComponent &textComp) {
        Renderer::setTextOrigin(0, 0);
        if (textComp.fontPath.size()) {
            auto font = Asset::manager().load<Font>(textComp.fontPath);
            Renderer::drawText(*font, textComp.text, textComp.pixelSize,
                               transformComp.transform.getWorldTransform(),
                               textComp.color);
        }
    });
    Renderer::endScene();
}

}  // namespace sd
