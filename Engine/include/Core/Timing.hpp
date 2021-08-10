#ifndef SD_TIMING_HPP
#define SD_TIMING_HPP

#include <cstdint>

namespace sd {

class Clock {
   public:
    Clock();

    uint32_t getElapsedTime() const;

    uint32_t restart();

   private:
    uint32_t m_lastTicks;
};

class FpsCounter {
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
