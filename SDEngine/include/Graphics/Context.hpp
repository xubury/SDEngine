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
};

}  // namespace SD
#endif
