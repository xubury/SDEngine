#include "Core/Keyboard.hpp"

#include <SDL.h>

namespace SD {

Scancode getScancodeFromKeycode(Keycode keycode) {
    return static_cast<Scancode>(
        SDL_GetScancodeFromKey(static_cast<SDL_KeyCode>(keycode)));
}

Keycode getKeycodeFromScancode(Scancode scancode) {
    return static_cast<Keycode>(
        SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(scancode)));
}

}  // namespace SD
