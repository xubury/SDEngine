#ifndef SD_RESOURCE_HPP
#define SD_RESOURCE_HPP

#include <string>
#include "Resource/ResourceId.hpp"

namespace SD {

class SD_RESOURCE_API Resource {
   public:
    virtual ~Resource() = default;

    const std::string &Name() const { return m_name; }
    const ResourceId Id() const { return m_id; }

   private:
    friend class ResourceManager;

    std::string m_name;
    ResourceId m_id;
};

}  // namespace SD
#endif
