#ifndef SD_TIMING_HPP
#define SD_TIMING_HPP

#include "Utility/Base.hpp"

#include <cstdint>
#include <queue>
#include <chrono>

namespace SD {

using ClockType = std::chrono::high_resolution_clock;

class SD_UTILITY_API Clock {
   public:
    Clock();

    float GetElapsedMS() const;

    // Restart the clock, and return elapsed millisecond.
    float Restart();

   private:
    std::chrono::time_point<ClockType> m_lastTicks;
};

class SD_UTILITY_API FPSCounter {
   public:
    FPSCounter(uint8_t capacity);

    FPSCounter(const FPSCounter &) = delete;
    FPSCounter &operator=(const FPSCounter &) = delete;

    float GetFPS() const;
    float GetFrameTime() const;

    void Probe();

   private:
    Clock m_clock;
    std::deque<float> m_queue;
    uint8_t m_capacity;
};

}  // namespace SD

#endif /* SD_TIMING_HPP */
