#include "Utility/Random.hpp"

namespace SD {

std::mt19937 Random::s_randomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_distribution;

void Random::Init() {
    std::random_device rd;
    s_randomEngine.seed(rd());
}

void Random::init(uint32_t seed) { s_randomEngine.seed(seed); }

double Random::Uniform() {
    return static_cast<double>(s_distribution(s_randomEngine)) /
           std::numeric_limits<uint32_t>::max();
}

float Random::Rnd(float min, float max) {
    return Uniform() * (max - min) + min;
}

double Random::Rnd(double min, double max) {
    return Uniform() * (max - min) + min;
}

int Random::Rnd(int min, int max) { return Uniform() * (max - min) + min; }

}  // namespace SD
