#include "Utility/ResourceId.hpp"
#include <random>
#include <unordered_map>

namespace std {
template <>
struct hash<sd::ResourceId> {
    std::size_t operator()(const sd::ResourceId &resourceID) {
        return hash<uint64_t>()(static_cast<uint64_t>(resourceID));
    }
};
}  // namespace std

namespace sd {

static std::random_device s_randomDevice;
static std::mt19937_64 s_randomEngine(s_randomDevice());
static std::uniform_int_distribution<uint64_t> s_uniformDistribution;

static std::unordered_map<ResourceId, std::string> s_map;

ResourceId::ResourceId() : m_resourceID(s_uniformDistribution(s_randomEngine)) {}

ResourceId::ResourceId(uint64_t resourceID) : m_resourceID(resourceID) {}

}  // namespace sd
