#include "Renderer/SpriteLoader.hpp"
#include "Renderer/Sprite.hpp"
#include "Renderer/Bitmap.hpp"
#include "Utility/String.hpp"

namespace SD {

Ref<void> SpriteLoader::LoadAsset(const std::string& path) {
    Ref<Bitmap> bitmap = Manager().LoadAndGet<Bitmap>(path);
    Ref<Sprite> sprite = CreateRef<Sprite>();
    if (bitmap == nullptr) throw Exception("Invalid image");
    auto texture = Texture::Create(
        bitmap->Width(), bitmap->Height(),
        TextureSpec(1, TextureType::TEX_2D,
                    bitmap->HasAlpha() ? DataFormat::RGBA : DataFormat::RGB,
                    DataFormatType::UBYTE, TextureWrap::EDGE));
    texture->SetPixels(0, 0, 0, bitmap->Width(), bitmap->Height(), 1,
                       bitmap->Data());
    sprite->SetTexture(texture);
    return sprite;
}

}  // namespace SD
