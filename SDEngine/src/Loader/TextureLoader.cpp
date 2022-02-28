#include "Loader/TextureLoader.hpp"
#include "Graphics/Texture.hpp"
#include "Loader/SDLHelper.hpp"
#include "Utility/String.hpp"

#include <SDL_image.h>
#include <fstream>

namespace SD {

Ref<Texture> TextureLoader::LoadTextureCube(
    const std::vector<std::string>& pathes) {
    Ref<Texture> texture;
    if (pathes.size() < 6) {
        throw Exception("Not enough images to load a texture cube!");
    }
    for (int face = 0; face < 6; ++face) {
        SDL_Surface* surface = LoadImage(pathes[face]);
        if (face == 0) {
            auto spec =
                TextureSpec(1, TextureType::TEX_CUBE, GetDataFormat(surface),
                            GetDataFormatType(surface), TextureWrap::EDGE);
            texture = Texture::Create(surface->w, surface->h, spec);
        }
        texture->SetPixels(0, 0, face, surface->w, surface->h, 1,
                           surface->pixels);
    }
    return texture;
}

Ref<Texture> TextureLoader::LoadTexture2D(const std::string& path) {
    SDL_Surface* surface = LoadImage(path);
    auto spec = TextureSpec(1, TextureType::TEX_2D, GetDataFormat(surface),
                            GetDataFormatType(surface), TextureWrap::EDGE);

    Ref<Texture> texture = Texture::Create(surface->w, surface->h, spec);
    texture->SetPixels(0, 0, 0, surface->w, surface->h, 1, surface->pixels);
    SDL_FreeSurface(surface);
    return texture;
}

}  // namespace SD
