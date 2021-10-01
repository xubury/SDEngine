#ifndef SD_CONTEXT_HPP
#define SD_CONTEXT_HPP

#include "Utility/Export.hpp"
#include "Utility/Base.hpp"
#include "Core/Window.hpp"
#include <SDL.h>

namespace sd {

class SD_API Context {
   public:
    static Ref<Context> create(const WindowProp &property, SDL_Window **window);

    virtual ~Context() = default;

    virtual void *getHandle() const = 0;

    virtual void swapBuffer() const = 0;

    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;

   protected:
    Context() = default;
};

}  // namespace sd
#endif /* SD_CONTEXT_HPP */
