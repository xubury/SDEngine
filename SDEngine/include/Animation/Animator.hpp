#ifndef SD_ANIMATOR_HPP
#define SD_ANIMATOR_HPP

#include "Animation/Export.hpp"
#include "Animation/Animation.hpp"

namespace SD {

class SD_ANIMATION_API Animator {
   public:
    Animator();

    void Play();

    void Stop();

    void Tick(Animation *animation, float dt);

    bool IsPlaying() const { return m_is_playing; }

   private:
    bool m_is_playing;
};

}  // namespace SD

#endif
