#include "Utils/Random.hpp"

namespace sd {

std::mt19937 Random::s_randomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_distribution;

void Random::init() {
    std::random_device rd;
    s_randomEngine.seed(rd());
}

void Random::init(uint32_t seed) { s_randomEngine.seed(seed); }

float Random::rnd(float min, float max) {
    return (float)s_distribution(s_randomEngine) /
               (float)std::numeric_limits<uint32_t>::max() * (max - min) +
           min;
}

double Random::rnd(double min, double max) {
    return (double)s_distribution(s_randomEngine) /
               (double)std::numeric_limits<uint32_t>::max() * (max - min) +
           min;
}

int Random::rnd(int min, int max) {
    return (float)s_distribution(s_randomEngine) /
               (float)std::numeric_limits<uint32_t>::max() * (max - min) +
           min;
}

}  // namespace sd
