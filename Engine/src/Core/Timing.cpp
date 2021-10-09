#include "Core/Timing.hpp"
#include <numeric>
#include <SDL.h>

namespace sd {

Clock::Clock() : m_lastTicks(SDL_GetTicks()) {}

uint32_t Clock::getElapsedMS() const { return SDL_GetTicks() - m_lastTicks; }

uint32_t Clock::restart() {
    uint32_t elapsed = getElapsedMS();
    m_lastTicks = SDL_GetTicks();
    return elapsed;
}

FpsCounter::FpsCounter(uint8_t capacity) : m_capacity(capacity) {}

float FpsCounter::getFps() {
    float fps = 1000.f / (float)m_clock.restart();
    m_queue.push_back(fps);
    if (m_queue.size() > m_capacity) {
        m_queue.pop_front();
    }

    return std::accumulate(m_queue.begin(), m_queue.end(), 0.f) / m_capacity;
}

}  // namespace sd
