#include "Animation/Animator.hpp"

#include <cmath>

namespace SD {

Animator::Animator() : m_is_playing(false) {}

void Animator::Play() {
    m_is_playing = true;
}

void Animator::Stop() { m_is_playing = false; }

void Animator::Tick(Animation *animation, float dt) {
    if (animation && m_is_playing) {
        animation->Tick(dt);
    }
}

}  // namespace SD
