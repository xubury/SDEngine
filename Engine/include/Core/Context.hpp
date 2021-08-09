#ifndef SD_CONTEXT_HPP
#define SD_CONTEXT_HPP

#include "Utils/Export.hpp"
#include "Utils/Base.hpp"
#include <SDL.h>

namespace sd {

class SD_API Context {
   public:
    static Ref<Context> create(SDL_Window *window);

    virtual ~Context() = default;

    virtual void *getHandle() const = 0;

   protected:
    Context() = default;
};

}  // namespace sd
#endif /* SD_CONTEXT_HPP */
