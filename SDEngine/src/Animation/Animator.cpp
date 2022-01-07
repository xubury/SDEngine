#include "Animation/Animator.hpp"

#include <cmath>

namespace SD {

Animator::Animator()
    : m_current_time(0), m_animation(nullptr), m_is_playing(false) {}

void Animator::Play(Animation *animation) {
    m_current_time = 0;
    m_animation = animation;
    m_is_playing = true;
}

void Animator::Stop() { m_is_playing = false; }

void Animator::Tick(float dt) {
    if (m_animation && m_is_playing) {
        m_current_time += dt;
        m_current_time = std::fmod(m_current_time, m_animation->GetDuration());
        m_animation->SetFrameTime(m_current_time);
    }
}

}  // namespace SD
