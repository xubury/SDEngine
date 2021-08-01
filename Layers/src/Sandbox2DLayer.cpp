#include "Sandbox2DLayer.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer2D.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Texture.hpp"
#include <SDL2/SDL_image.h>

SDL_Surface *loadSuface(const std::string &path) {
    // Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
               IMG_GetError());
    }
    return loadedSurface;
}

sd::Ref<sd::Texture> texture;

Sandbox2DLayer::Sandbox2DLayer() : sd::Layer("Sandbox2D") {}

void Sandbox2DLayer::onAttach() {
    SDL_Surface *image = loadSuface("assets/images/image.png");
    texture = sd::Texture::create(
        image->w, image->h, sd::TextureType::TEX_2D, sd::TextureFormat::RGB,
        sd::TextureFormatType::UBYTE, sd::TextureWrap::BORDER,
        sd::TextureFilter::NEAREST, sd::TextureMipmapFilter::NEAREST_LINEAR,
        image->pixels);
}

void Sandbox2DLayer::onTick(float) {}

void Sandbox2DLayer::onRender() {
    sd::RendererAPI::setClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    sd::RendererAPI::clear();
    sd::Renderer2D::beginScene(sd::OrthographicCamera(3, 3, -1.0f, 1.0f));
    if (texture) sd::Renderer2D::drawTexture(glm::mat4(1.0f), texture);
    sd::Renderer2D::drawQuad(
        glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 1.0f)),
        glm::vec4(1, 0, 0, 0.5));
    sd::Renderer2D::endScene();
}

void Sandbox2DLayer::onEventPoll(const SDL_Event &) {}

void Sandbox2DLayer::onEventProcess() {}
