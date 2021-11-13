#ifndef SD_EVENT_HPP
#define SD_EVENT_HPP

#include "Utility/Base.hpp"

namespace SD {

struct SD_API SizeEvent {
    SizeEvent(int width, int height) : width(width), height(height) {}
    int width;
    int height;
};

}  // namespace SD

#endif /* SD_EVENT_HPP */
