#include "Animation/Animation.hpp"

namespace SD {

void SpriteAnimation::SetFrameTime(float time) {
    m_frame_index = time * m_frame_per_second;
}

void SpriteAnimation::SetSpeed(float fps) {
    m_frame_per_second = fps;
    SetDuration(m_sprites.size() / m_frame_per_second);
}

void SpriteAnimation::PushBack(const Ref<Sprite> &sprite) {
    m_sprites.push_back(sprite);
    SetDuration(m_sprites.size() / m_frame_per_second);
}

void SpriteAnimation::SetFrame(const Ref<Sprite> &sprite, int index) {
    m_sprites.at(index) = sprite;
}

const Texture *SpriteAnimation::GetTexture() const {
    return m_sprites.at(m_frame_index)->GetTexture();
}

}  // namespace SD
