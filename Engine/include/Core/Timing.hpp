#ifndef SD_TIMING_HPP
#define SD_TIMING_HPP

#include <cstdint>
#include "Utils/Export.hpp"

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
    FpsCounter() = default;
    ~FpsCounter() = default;

    FpsCounter(const FpsCounter &) = delete;
    FpsCounter &operator=(const FpsCounter &) = delete;

    void begin();
    float end();

    float getFps() const;

   private:
    void calculateFps();

    float m_fps;
    Clock m_clock;
};

}  // namespace sd

#endif /* SD_TIMING_HPP */
