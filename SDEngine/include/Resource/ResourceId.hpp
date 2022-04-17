#ifndef SD_RESOURCE_ID_HPP
#define SD_RESOURCE_ID_HPP

#include "Resource/Export.hpp"
#include "Utility/Serialize.hpp"

#include <cstdint>
#include <unordered_map>

namespace SD {

class SD_RESOURCE_API ResourceId {
   public:
    ResourceId();
    ResourceId(uint64_t resource_id);
    ResourceId(const char *c_str);
    ResourceId(const std::string_view &str);
    ResourceId(const std::string &str);
    ResourceId(const ResourceId &) = default;
    ~ResourceId() = default;

    operator uint64_t() const { return m_resource_id; }

    SERIALIZE(m_resource_id)

   private:
    uint64_t m_resource_id;
};

}  // namespace SD

namespace std {
template <>
struct hash<SD::ResourceId> {
    std::size_t operator()(const SD::ResourceId &resource_id) const
    {
        return hash<uint64_t>()(static_cast<uint64_t>(resource_id));
    }
};
}  // namespace std

#endif
