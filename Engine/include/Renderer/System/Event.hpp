#ifndef SD_EVENT_HPP
#define SD_EVENT_HPP

namespace sd {

struct SizeEvent {
    SizeEvent(int width, int height) : width(width), height(height) {}
    int width;
    int height;
};

}  // namespace sd

#endif /* SD_EVENT_HPP */
