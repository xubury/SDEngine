#include "Core/Context.hpp"
#include "Core/OpenGL/GLContext.hpp"
#include "Graphics/Graphics.hpp"
#include "Utils/Log.hpp"

namespace sd {

Ref<Context> Context::create(const WindowProp &property, SDL_Window **window) {
    Ref<Context> context;
    switch (Graphics::getAPI()) {
        case API::OpenGL:
            context = createRef<GLContext>(property, window);
            break;
        default:
            SD_CORE_ERROR("Unsupported API!");
            break;
    }
    return context;
}

}  // namespace sd
