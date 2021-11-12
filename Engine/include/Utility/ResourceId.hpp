#ifndef SD_UUID_HPP
#define SD_UUID_HPP

#include "Utility/Base.hpp"

#include <cstdint>
#include <unordered_map>

namespace SD {

class SD_API ResourceId {
   public:
    ResourceId();
    ResourceId(uint64_t resourceID);
    ResourceId(const ResourceId &) = default;
    ~ResourceId() = default;

    operator uint64_t() const { return m_resourceID; }

    template <class Archive>
    void serialize(Archive &archive) {
        archive(m_resourceID);
    }

   private:
    uint64_t m_resourceID;
};

}  // namespace SD

namespace std {
template <>
struct hash<SD::ResourceId> {
    std::size_t operator()(const SD::ResourceId &resourceID) const {
        return hash<uint64_t>()(static_cast<uint64_t>(resourceID));
    }
};
}  // namespace std

#endif
