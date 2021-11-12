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

    uint64_t getElapsedMS() const;

    uint64_t getElapsedSec() const;

    // Restart the clock, and return elapsed millisecond.
    uint64_t restart();

   private:
    std::chrono::time_point<ClockType> m_lastTicks;
};

class SD_API FpsCounter {
   public:
    FpsCounter(uint8_t capacity);

    FpsCounter(const FpsCounter &) = delete;
    FpsCounter &operator=(const FpsCounter &) = delete;

    float getFps();

   private:
    Clock m_clock;
    std::deque<float> m_queue;
    uint8_t m_capacity;
};

}  // namespace SD

#endif /* SD_TIMING_HPP */
