#ifndef SD_GRAPHICS_RESOURCE_HPP
#define SD_GRAPHICS_RESOURCE_HPP

#include <cstdint>

namespace SD {

class Resource {
   public:
    virtual uint32_t GetId() const = 0;
};

}  // namespace SD
#endif
