#ifndef SD_ANIMATOR_HPP
#define SD_ANIMATOR_HPP

#include "Animation/Export.hpp"
#include "Animation/Animation.hpp"

namespace SD {

template <typename ANIM>
class Animator {
   public:
    Animator() : m_is_playing(false), m_animation(nullptr) {}

    void Play(ANIM *animation)
    {
        static_assert(std::is_base_of_v<Animation, ANIM>,
                      "Template must be derived from Animation class");
        m_is_playing = true;
        m_animation = animation;
    }

    void Stop() { m_is_playing = false; }

    void Tick(float dt)
    {
        if (m_animation && m_is_playing) {
            m_animation->Tick(dt);
        }
    }

    bool IsPlaying() const { return m_is_playing; }

    const ANIM *GetAnimation() const { return m_animation; }

   private:
    bool m_is_playing;
    ANIM *m_animation;
};

}  // namespace SD

#endif
