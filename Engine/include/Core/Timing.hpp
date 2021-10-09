#ifndef SD_TIMING_HPP
#define SD_TIMING_HPP

#include <cstdint>
#include <queue>
#include "Utility/Export.hpp"

namespace sd {

class SD_API Clock {
   public:
    Clock();

    uint32_t getElapsedMS() const;

    // Restart the clock, and return elapsed millisecond.
    uint32_t restart();

   private:
    uint32_t m_lastTicks;
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

}  // namespace sd

#endif /* SD_TIMING_HPP */
