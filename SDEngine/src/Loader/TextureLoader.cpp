#include "Loader/TextureLoader.hpp"
#include "Graphics/Texture.hpp"
#include "Loader/SDLHelper.hpp"
#include "Utility/String.hpp"

#include <SDL_image.h>

namespace SD {

static Ref<Texture> CreateTexture2D(const std::string& path) {
    SDL_Surface* surface = LoadImage(path);
    auto spec = TextureSpec(1, TextureType::TEX_2D, GetDataFormat(surface),
                            GetDataFormatType(surface), TextureWrap::EDGE);

    Ref<Texture> texture = Texture::Create(surface->w, surface->h, spec);
    texture->SetPixels(0, 0, 0, surface->w, surface->h, 1, surface->pixels);
    SDL_FreeSurface(surface);
    return texture;
}

static Ref<Texture> CreateTextureCube(const std::string& path) {
    std::ifstream file;
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    try {
        file.open(path);
    } catch (std::ifstream::failure& e) {
        throw FileException(path, std::strerror(errno));
    }
    file.exceptions(std::ifstream::badbit);
    const auto directory = std::filesystem::path(path).parent_path();
    int line_number = 0;
    int face = 0;
    Ref<Texture> texture;
    for (std::string line; std::getline(file, line); ++line_number, ++face) {
        String::Trim(line);
        SDL_Surface* surface = LoadImage((directory / line).string());
        if (face == 0) {
            auto spec =
                TextureSpec(1, TextureType::TEX_CUBE, GetDataFormat(surface),
                            GetDataFormatType(surface), TextureWrap::EDGE);
            texture = Texture::Create(surface->w, surface->h, spec);
        }
        texture->SetPixels(0, 0, face, surface->w, surface->h, 1,
                           surface->pixels);

        SDL_FreeSurface(surface);
    }
    return texture;
}

Ref<void> TextureLoader::LoadAsset(const std::string& path) {
    size_t pos = path.find_last_of('.');
    std::string ext;
    Ref<Texture> texture;
    if (pos != std::string::npos) {
        ext = path.substr(pos);
        if (ext == ".cube") {
            texture = CreateTextureCube(path);
        } else {
            texture = CreateTexture2D(path);
        }
    } else {
        throw Exception("Unknown file extension!");
    }
    return texture;
}

}  // namespace SD
