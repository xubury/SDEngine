#include "Utility/Timing.hpp"
#include <numeric>

namespace SD {

Clock::Clock() : m_lastTicks(ClockType::now()) {}

uint64_t Clock::getElapsedMS() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               ClockType::now() - m_lastTicks)
        .count();
}

uint64_t Clock::getElapsedSec() const {
    return std::chrono::duration_cast<std::chrono::seconds>(ClockType::now() -
                                                            m_lastTicks)
        .count();
}

uint64_t Clock::restart() {
    uint32_t elapsed = getElapsedMS();
    m_lastTicks = ClockType::now();
    return elapsed;
}

FpsCounter::FpsCounter(uint8_t capacity) : m_capacity(capacity) {}

float FpsCounter::getFps() {
    float fps = 1000.f / static_cast<float>(m_clock.restart());
    m_queue.push_back(fps);
    if (m_queue.size() > m_capacity) {
        m_queue.pop_front();
    }

    return std::accumulate(m_queue.begin(), m_queue.end(), 0.f) / m_capacity;
}

}  // namespace SD
