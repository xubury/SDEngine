#ifndef SD_CONTEXT_HPP
#define SD_CONTEXT_HPP

#include "Graphics/Export.hpp"
#include "Graphics/Viewport.hpp"

#include <glm/glm.hpp>

namespace SD {

class SD_GRAPHICS_API Context {
   public:
    Context() = default;
    virtual ~Context() = default;
    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;

    virtual glm::ivec2 GetSize() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual uint8_t GetMSAA() const = 0;
};

}  // namespace SD
#endif
