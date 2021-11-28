#ifndef SD_TIMING_HPP
#define SD_TIMING_HPP

#include "Utility/Base.hpp"

#include <cstdint>
#include <queue>
#include <chrono>

namespace SD {

using ClockType = std::chrono::high_resolution_clock;

class SD_API Clock {
   public:
    Clock();

    uint64_t GetElapsedMS() const;

    uint64_t GetElapsedSec() const;

    // Restart the clock, and return elapsed millisecond.
    uint64_t Restart();

   private:
    std::chrono::time_point<ClockType> m_lastTicks;
};

class SD_API FPSCounter {
   public:
    FPSCounter(uint8_t capacity);

    FPSCounter(const FPSCounter &) = delete;
    FPSCounter &operator=(const FPSCounter &) = delete;

    float GetFPS();

   private:
    Clock m_clock;
    std::deque<float> m_queue;
    uint8_t m_capacity;
};

}  // namespace SD

#endif /* SD_TIMING_HPP */
