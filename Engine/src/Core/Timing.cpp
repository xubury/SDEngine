#include "Core/Timing.hpp"
#include <SDL.h>

namespace sd {

Clock::Clock() : m_lastTicks(SDL_GetTicks()) {}

uint32_t Clock::getElapsedMS() const { return SDL_GetTicks() - m_lastTicks; }

uint32_t Clock::restart() {
    uint32_t elapsed = getElapsedMS();
    m_lastTicks = SDL_GetTicks();
    return elapsed;
}

float FpsCounter::getFps() {
    uint32_t elapsed = m_clock.restart();
    return 1000.f / elapsed;
}

}  // namespace sd
