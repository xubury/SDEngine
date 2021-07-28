#ifndef SD_TIMING_HPP
#define SD_TIMING_HPP

#include <stdint.h>

namespace sd {

class FpsLimiter {
   public:
    FpsLimiter() = default;
    ~FpsLimiter() = default;

    FpsLimiter(const FpsLimiter &) = delete;
    FpsLimiter &operator=(const FpsLimiter &) = delete;

    void init(float maxFps);
    void setMaxFps(float maxFps);

    void begin();
    float end();

   private:
    void calculateFps();

    float m_fps;
    float m_maxFps;
    float m_frameTime;
    uint32_t m_startTicks;
};

}  // namespace sd

#endif /* SD_TIMING_HPP */
