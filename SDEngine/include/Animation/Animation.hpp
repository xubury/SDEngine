#ifndef SD_ANIMATION_HPP
#define SD_ANIMATION_HPP

#include "Animation/Export.hpp"
#include "Renderer/Sprite.hpp"

#include <cstdint>

namespace SD {

class SD_ANIMATION_API Animation {
   public:
    Animation() = default;

    float GetDuration() const { return m_duration; }
    void SetDuration(float duration) { m_duration = duration; }

    virtual void SetFrameTime(float time) = 0;

   private:
    float m_duration;
};

class SD_ANIMATION_API SpriteAnimation : public Animation {
   public:
    SpriteAnimation() = default;

    void SetFrameTime(float time) override;

    void SetSpeed(float fps);

    void SetFrameIndex(int index) { m_frame_index = index; }

    void PushBack(const Ref<Sprite>& sprite);

    void SetFrame(const Ref<Sprite>& sprite, int index);

    const Texture* GetTexture() const;

   private:
    float m_frame_per_second;
    uint32_t m_frame_index;

    std::vector<Ref<Sprite>> m_sprites;
};

}  // namespace SD
#endif
