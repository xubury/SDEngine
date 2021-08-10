#include "Core/Context.hpp"
#include "Core/OpenGL/GLContext.hpp"
#include "Graphics/Graphics.hpp"
#include "Common/Log.hpp"

namespace sd {

Ref<Context> Context::create(SDL_Window *window) {
    Ref<Context> context;
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            context = createRef<GLContext>(window);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return context;
}

}  // namespace sd
