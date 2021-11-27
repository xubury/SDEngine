#include "Utility/Timing.hpp"
#include <numeric>

namespace SD {

Clock::Clock() : m_lastTicks(ClockType::now()) {}

uint64_t Clock::GetElapsedMS() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               ClockType::now() - m_lastTicks)
        .count();
}

uint64_t Clock::GetElapsedSec() const {
    return std::chrono::duration_cast<std::chrono::seconds>(ClockType::now() -
                                                            m_lastTicks)
        .count();
}

uint64_t Clock::Restart() {
    uint32_t elapsed = GetElapsedMS();
    m_lastTicks = ClockType::now();
    return elapsed;
}

FpsCounter::FpsCounter(uint8_t capacity) : m_capacity(capacity) {}

float FpsCounter::GetFPS() {
    float fps = 1000.f / static_cast<float>(m_clock.Restart());
    m_queue.push_back(fps);
    if (m_queue.size() > m_capacity) {
        m_queue.pop_front();
    }

    return std::accumulate(m_queue.begin(), m_queue.end(), 0.f) / m_queue.size();
}

}  // namespace SD
