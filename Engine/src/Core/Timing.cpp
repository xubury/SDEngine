#include "Core/Timing.hpp"
#include <SDL.h>

namespace sd {

Clock::Clock() : m_lastTicks(SDL_GetTicks()) {}

uint32_t Clock::getElapsedTime() const { return SDL_GetTicks() - m_lastTicks; }

uint32_t Clock::restart() {
    uint32_t elapsed = getElapsedTime();
    m_lastTicks = SDL_GetTicks();
    return elapsed;
}

void FpsCounter::begin() { m_clock.restart(); }

float FpsCounter::end() {
    calculateFps();
    return m_fps;
}

void FpsCounter::calculateFps() { m_fps = 1000.f / m_clock.getElapsedTime(); }

float FpsCounter::getFps() const { return m_fps; }

}  // namespace sd
