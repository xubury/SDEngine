#ifndef SD_MOUSE_HPP
#define SD_MOUSE_HPP

#include <cstdint>

namespace SD {

enum class MouseButton : uint8_t {
    Left = 1,
    Middle = 2,
    Right = 3,
    X1 = 4,
    X2 = 5,
};

}

#endif
