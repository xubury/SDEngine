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

template <typename FRAME>
class FrameAnimation : public Animation {
   public:
    FrameAnimation() : m_frame_index(0), m_frame_per_second(30.0f){};

    void SetFrameTime(float time) override {
        m_frame_index = time * m_frame_per_second;
    }

    void SetSpeed(float fps) {
        m_frame_per_second = fps;
        SetDuration(m_frames.size() / m_frame_per_second);
    }

    float GetSpeed() const { return m_frame_per_second; }

    void SetFrameIndex(int index) { m_frame_index = index; }

    void PushBack(const FRAME& frame) {
        m_frames.push_back(frame);
        SetDuration(m_frames.size() / m_frame_per_second);
    }

    void SetFrame(const FRAME& frame, int index) { m_frames.at(index) = frame; }

    const FRAME& GetFrame(int index = -1) const {
        return index == -1 ? m_frames.at(m_frame_index) : m_frames.at(index);
    }
    size_t GetFrameSize() const { return m_frames.size(); }

    SERIALIZE(m_frame_index, m_frame_per_second, m_frames)

   private:
    int m_frame_index;
    float m_frame_per_second;

    std::vector<FRAME> m_frames;
};

}  // namespace SD
#endif
