#include "Utility/Random.hpp"

namespace SD {

static std::random_device s_random_device;
static std::mt19937 s_random_engine(s_random_device());
static std::uniform_real_distribution<double> s_distribution(0.0, 1.0);

void Random::Init(uint32_t seed) { s_random_engine.seed(seed); }

double Random::Uniform() { return s_distribution(s_random_engine); }

float Random::Rnd(float min, float max)
{
    return Uniform() * (max - min) + min;
}

double Random::Rnd(double min, double max)
{
    return Uniform() * (max - min) + min;
}

int Random::Rnd(int min, int max) { return Uniform() * (max - min) + min; }

}  // namespace SD
