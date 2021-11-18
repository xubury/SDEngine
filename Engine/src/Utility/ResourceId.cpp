#include "Utility/ResourceId.hpp"
#include <random>
#include <unordered_map>

namespace SD {

static std::random_device s_random_device;
static std::mt19937_64 s_random_engine(s_random_device());
static std::uniform_int_distribution<uint64_t> s_uniform_distribution;

ResourceId::ResourceId()
    : m_resource_id(s_uniform_distribution(s_random_engine)) {}

ResourceId::ResourceId(uint64_t resource_id) : m_resource_id(resource_id) {}

}  // namespace SD
