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
    float GetSpeed() const { return m_frame_per_second; }

    void SetFrameIndex(int index) { m_frame_index = index; }

    void PushBack(const Ref<Sprite>& sprite,
                  const std::array<glm::vec2, 2>& uvs);

    void SetFrame(const Ref<Sprite>& sprite,
                  const std::array<glm::vec2, 2>& uvs, int index);

    const Texture* GetTexture(int index = -1) const;
    const std::array<glm::vec2, 2>& GetUVs(int index = -1) const;

    size_t GetSize() const { return m_sprites.size(); }

    SERIALIZE(m_frame_per_second, m_frame_index, m_uvs)

   private:
    float m_frame_per_second;
    uint32_t m_frame_index;

    std::vector<Ref<Sprite>> m_sprites;
    std::vector<std::array<glm::vec2, 2>> m_uvs;
};

}  // namespace SD
#endif
