#ifndef SD_RANDOM_HPP
#define SD_RANDOM_HPP

#include "Utility/Base.hpp"

#include <cstdint>
#include <random>

namespace SD {

class SD_UTILITY_API Random {
   public:
    static float Rnd(float min, float max);
    static double Rnd(double min, double max);
    static int Rnd(int min, int max);

    static void Init(uint32_t seed);

   private:
    static double Uniform();
};

}  // namespace SD

#endif /* SD_RANDOM_HPP */
