#include "Utility/Timing.hpp"
#include <numeric>

namespace SD {

Clock::Clock() : m_lastTicks(ClockType::now()) {}

float Clock::GetElapsedMS() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
               ClockType::now() - m_lastTicks)
               .count() /
           1000.f;
}

float Clock::Restart()
{
    float elapsed = GetElapsedMS();
    m_lastTicks = ClockType::now();
    return elapsed;
}

FPSCounter::FPSCounter(uint8_t capacity) : m_capacity(capacity) {}

void FPSCounter::Probe()
{
    float time = static_cast<float>(m_clock.Restart());
    m_queue.push_back(time);
    if (m_queue.size() > m_capacity) {
        m_queue.pop_front();
    }
}

float FPSCounter::GetFPS() const { return 1000 / GetFrameTime(); }

float FPSCounter::GetFrameTime() const
{
    return std::accumulate(m_queue.begin(), m_queue.end(), 0.f) /
           m_queue.size();
}

}  // namespace SD
