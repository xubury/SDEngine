#include "Core/Timing.hpp"
#include <SDL2/SDL.h>

namespace sd {

void FpsLimiter::init(float maxFps) { setMaxFps(maxFps); }

void FpsLimiter::setMaxFps(float maxFps) { m_maxFps = maxFps; }

void FpsLimiter::begin() { m_startTicks = SDL_GetTicks(); }

float FpsLimiter::end() {
    float frameTicks = SDL_GetTicks() - m_startTicks;

    float delay = 1000.f / m_maxFps - frameTicks;
    if (delay > 0.f) {
        SDL_Delay(delay);
    }

    calculateFps();
    return m_fps;
}

void FpsLimiter::calculateFps() {
    m_frameTicks = SDL_GetTicks() - m_startTicks;
    m_fps = 1000.f / m_frameTicks;
}

}  // namespace sd
