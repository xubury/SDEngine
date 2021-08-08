#ifndef SD_RANDOM_HPP
#define SD_RANDOM_HPP

#include <stdint.h>
#include <random>

namespace sd {

class Random {
   public:
    static float rnd(float min, float max);
    static double rnd(double min, double max);
    static int rnd(int min, int max);

    static void init();
    static void init(uint32_t seed);

   private:
    static std::mt19937 s_randomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type>
        s_distribution;
};

}  // namespace sd

#endif /* SD_RANDOM_HPP */
