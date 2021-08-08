#ifndef SD_RANDOM_HPP
#define SD_RANDOM_HPP

#include <stdint.h>
#include <random>

namespace sd {

class Random {
   public:
    double rnd(double min, double max);
    int rnd(int min, int max);

    static void init();
    static void init(uint32_t seed);

   private:
    static uint32_t s_nProcGen;
    static uint32_t lehmer64();
};

}  // namespace sd

#endif /* SD_RANDOM_HPP */
