#include "Renderer/System/SpriteRenderSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Utility/Loader/AssetLoader.hpp"

namespace sd {

SpriteRenderSystem::SpriteRenderSystem() {}

void SpriteRenderSystem::onInit() {}

void SpriteRenderSystem::onDestroy() {}

void SpriteRenderSystem::onRender() {
    auto scene = Renderer::engine().getScene();
    auto textView = scene->view<TransformComponent, TextComponent>();

    Renderer::engine().getRenderTarget().bind();
    Renderer2D::beginScene(*Renderer::engine().getCamera());
    textView.each([](const TransformComponent &transformComp,
                     const TextComponent &textComp) {
        Renderer2D::setTextOrigin(0, 0);
        if (textComp.fontPath.size()) {
            auto font = Asset::manager().load<Font>(textComp.fontPath);
            Renderer2D::drawText(*font, textComp.text, textComp.pixelSize,
                                 transformComp.transform.getWorldTransform(),
                                 textComp.color);
        }
    });
    Renderer2D::endScene();
}

}  // namespace sd
