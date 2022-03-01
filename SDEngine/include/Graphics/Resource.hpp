#ifndef SD_GRAPHICS_RESOURCE_HPP
#define SD_GRAPHICS_RESOURCE_HPP

#include <cstdint>

namespace SD {

class Resource {
   public:
    uint32_t GetId() const { return m_id; }

   protected:
    uint32_t m_id;
};

}  // namespace SD
#endif
