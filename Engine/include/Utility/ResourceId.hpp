#ifndef SD_UUID_HPP
#define SD_UUID_HPP

#include <cstdint>

namespace sd {

class ResourceId {
   public:
    ResourceId();
    ResourceId(uint64_t resourceID);
    ResourceId(const ResourceId &) = default;
    ~ResourceId() = default;

    operator uint64_t() const { return m_resourceID; }

    template <class Archive>
    void serialize(Archive& archive) {
        archive(m_resourceID);
    }
   private:
    uint64_t m_resourceID;
};

}  // namespace sd

#endif
