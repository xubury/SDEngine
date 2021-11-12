#ifndef SD_RANDOM_HPP
#define SD_RANDOM_HPP

#include "Utility/Base.hpp"

#include <cstdint>
#include <random>

namespace SD {

class SD_API Random {
   public:
    static float rnd(float min, float max);
    static double rnd(double min, double max);
    static int rnd(int min, int max);

    static void init();
    static void init(uint32_t seed);

   private:
    static double uniform();

    static std::mt19937 s_randomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type>
        s_distribution;
};

}  // namespace SD

#endif /* SD_RANDOM_HPP */
