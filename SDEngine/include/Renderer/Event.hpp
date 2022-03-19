#ifndef SD_RENDERER_EVENT_HPP
#define SD_RENDERER_EVENT_HPP

namespace SD {
struct RenderSizeEvent {
    int width;
    int height;
};

class Camera;
struct CameraEvent {
    Camera *camera;
};
}  // namespace SD

#endif
