#include "Animation/Animation.hpp"

namespace SD {

void SpriteAnimation::SetFrameTime(float time) {
    m_frame_index = time * m_frame_per_second;
}

void SpriteAnimation::SetSpeed(float fps) {
    m_frame_per_second = fps;
    SetDuration(m_sprites.size() / m_frame_per_second);
}

void SpriteAnimation::PushBack(const Ref<Sprite>& sprite,
                               const std::array<glm::vec2, 2>& uvs) {
    m_sprites.push_back(sprite);
    m_uvs.push_back(uvs);
    SetDuration(m_sprites.size() / m_frame_per_second);
}

void SpriteAnimation::SetFrame(const Ref<Sprite>& sprite,
                               const std::array<glm::vec2, 2>& uvs, int index) {
    m_sprites.at(index) = sprite;
    m_uvs.at(index) = uvs;
}

const Texture* SpriteAnimation::GetTexture(int index) const {
    return index == -1 ? m_sprites.at(m_frame_index)->GetTexture()
                       : m_sprites.at(index)->GetTexture();
}
const std::array<glm::vec2, 2>& SpriteAnimation::GetUVs(int index) const {
    return index == -1 ? m_uvs.at(m_frame_index) : m_uvs.at(m_frame_index);
}

}  // namespace SD
