#ifndef SD_ANIMATOR_HPP
#define SD_ANIMATOR_HPP

#include "Animation/Export.hpp"
#include "Animation/Animation.hpp"

namespace SD {

class SD_ANIMATION_API Animator {
   public:
    Animator();

    void Play(Animation *animation);

    void Stop();

    void Tick(float dt);

   private:
    float m_current_time;

    Animation *m_animation;

    bool m_is_playing;
};

}  // namespace SD

#endif
