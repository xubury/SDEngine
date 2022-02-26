#include "Loader/SpriteLoader.hpp"
#include "Renderer/Sprite.hpp"
#include "Loader/SDLHelper.hpp"
#include "Utility/String.hpp"

#include <SDL_image.h>

namespace SD {

Ref<void> SpriteLoader::LoadAsset(const std::string &path) {
    SDL_Surface *surface = LoadImage(path);
    int channels = GetNumOfChannels(surface);
    auto spec =
        TextureSpec(1, TextureType::TEX_2D, GetDataFormat(channels),
                    GetDataFormatType(GetBitsPerPixel(surface), channels),
                    TextureWrap::EDGE);

    Ref<Sprite> sprite = CreateRef<Sprite>(surface->w, surface->h, spec);
    sprite->GetTexture()->SetPixels(0, 0, 0, surface->w, surface->h, 1,
                                    surface->pixels);
    SDL_FreeSurface(surface);
    return sprite;
}

}  // namespace SD
