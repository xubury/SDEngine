#include "Renderer/Sprite.hpp"

namespace SD {

Sprite::Sprite(const Ref<Texture> &texture) : m_texture(texture) {
    SD_CORE_ASSERT(texture, "Invalid texture");
}

}  // namespace SD
