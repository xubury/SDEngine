#ifndef SD_EVENT_HPP
#define SD_EVENT_HPP

namespace SD {

struct SizeEvent {
    SizeEvent(int width, int height) : width(width), height(height) {}
    int width;
    int height;
};

}  // namespace SD

#endif /* SD_EVENT_HPP */
