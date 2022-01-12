#ifndef SD_ANIMATION_HPP
#define SD_ANIMATION_HPP

#include "Animation/Export.hpp"
#include "Renderer/Sprite.hpp"

#include <cstdint>

namespace SD {

class SD_ANIMATION_API Animation {
   public:
    Animation() : m_duration(0), m_is_loop(true){};

    float GetDuration() const { return m_duration; }
    void SetDuration(float duration) { m_duration = duration; }

    bool IsLoop() const { return m_is_loop; }
    void SetLoop(bool loop) { m_is_loop = loop; }

    SERIALIZE(m_duration, m_is_loop)

   protected:
    template <typename ANIM>
    friend class Animator;

    virtual void Tick(float dt) = 0;

    float m_duration;
    bool m_is_loop;
};

template <typename FRAME>
class FrameAnimation : public Animation {
   public:
    FrameAnimation()
        : m_frame_index(0), m_frame_per_second(30.0f), m_frame_progress(0.f){};

    void Tick(float time) override {
        float progess = m_frame_per_second * time;
        if (m_frame_index + 1 <= m_frame_progress + progess) {
            // new frame
            // callback();

            if (m_frame_index + 1 == m_frames.size()) {
                // finish
                if (m_is_loop) {
                    m_frame_index = 0;
                    m_frame_progress = 0.f;
                } else {
                    m_frame_progress = m_frames.size();
                }
            } else {
                ++m_frame_index;
                m_frame_progress += progess;
            }
        } else {
            m_frame_progress += progess;
        }
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

    SERIALIZE(m_frame_index, m_frame_per_second, m_frame_progress, m_frames)

   private:
    size_t m_frame_index;
    float m_frame_per_second;
    float m_frame_progress;

    std::vector<FRAME> m_frames;
};

}  // namespace SD
#endif
