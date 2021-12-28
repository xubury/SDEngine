#include "TileMap/TileMapLoader.hpp"
#include "TileMap/TileMap.hpp"
#include "Renderer/Bitmap.hpp"
#include "Utility/String.hpp"

namespace SD {

inline int GetValue(const std::string& str) {
    const char* value = str.c_str();
    char* end;
    long n = strtol(value, &end, 0);
    if (end <= value) throw Exception("Invalid size");
    return n;
}

Ref<void> TileMapLoader::LoadAsset(const std::string& path) {
    Ref<Bitmap> bitmap = Manager().LoadAndGet<Bitmap>(path);
    Ref<TileMap> tilemap = CreateRef<TileMap>();
    if (bitmap == nullptr) throw Exception("Invalid image");
    auto texture = Texture::Create(
        bitmap->Width(), bitmap->Height(),
        TextureSpec(1, TextureType::TEX_2D,
                    bitmap->HasAlpha() ? DataFormat::RGBA : DataFormat::RGB,
                    DataFormatType::UBYTE, TextureWrap::EDGE));
    texture->SetPixels(0, 0, 0, bitmap->Width(), bitmap->Height(), 1,
                       bitmap->Data());
    tilemap->SetTexture(texture);
    return tilemap;
}

}  // namespace SD
