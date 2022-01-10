#include "Animation/Animator.hpp"

#include <cmath>

namespace SD {

Animator::Animator() : m_current_time(0), m_is_playing(false) {}

void Animator::Play() {
    m_current_time = 0;
    m_is_playing = true;
}

void Animator::Stop() { m_is_playing = false; }

void Animator::Tick(Animation *animation, float dt) {
    if (animation && m_is_playing) {
        m_current_time += dt;
        m_current_time = std::fmod(m_current_time, animation->GetDuration());
        animation->SetFrameTime(m_current_time);
    }
}

}  // namespace SD
