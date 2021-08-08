#include "Utils/Random.hpp"

namespace sd {

uint32_t Random::s_nProcGen;

void Random::init() {
    std::random_device rd;
    s_nProcGen = rd();
}

uint32_t Random::lehmer64() {
    s_nProcGen += 0xe120fc15;
    uint64_t tmp;
    tmp = (uint64_t)s_nProcGen * 0x4a39b70d;
    uint32_t m1 = (tmp >> 32) ^ tmp;
    tmp = (uint64_t)m1 * 0x12fad5c9;
    uint32_t m2 = (tmp >> 32) ^ tmp;
    return m2;
}

void Random::init(uint32_t seed) { s_nProcGen = seed; }

double Random::rnd(double min, double max) {
    return ((double)lehmer64() / (double)(0x7FFFFFFF)) * (max - min) + min;
}

int Random::rnd(int min, int max) { return (lehmer64() % (max - min)) + min; }

}  // namespace sd
