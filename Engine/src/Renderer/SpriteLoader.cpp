#include "Renderer/SpriteLoader.hpp"
#include "Renderer/Bitmap.hpp"
#include "Renderer/Sprite.hpp"
#include "Utility/String.hpp"

namespace SD {

inline int GetValue(const std::string& str) {
    const char* value = str.c_str();
    char* end;
    long n = strtol(value, &end, 0);
    if (end <= value) throw Exception("Invalid size");
    return n;
}

Ref<void> SpriteLoader::LoadAsset(const std::string& path) {
    std::ifstream file;
    file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    try {
        file.open(path);
    } catch (std::ifstream::failure& e) {
        throw FileException(path, std::strerror(errno));
    }
    file.exceptions(std::ifstream::badbit);

    std::filesystem::path directory = std::filesystem::path(path).parent_path();
    std::string image_path;
    std::getline(file, image_path);

    String::Trim(image_path);
    Ref<Bitmap> bitmap = Manager().LoadAndGet<Bitmap>(directory / image_path);
    if (bitmap == nullptr) throw Exception("Invalid image");
    auto texture = Texture::Create(
        bitmap->Width(), bitmap->Height(),
        TextureSpec(1, TextureType::TEX_2D,
                    bitmap->HasAlpha() ? DataFormat::RGBA : DataFormat::RGB,
                    DataFormatType::UBYTE, TextureWrap::EDGE));
    texture->SetPixels(0, 0, 0, bitmap->Width(), bitmap->Height(), 1,
                       bitmap->Data());
    std::array<glm::vec2, 2> uvs;
    Ref<Sprite> sprite = CreateRef<Sprite>(texture, uvs);
    return sprite;
}

}  // namespace SD
