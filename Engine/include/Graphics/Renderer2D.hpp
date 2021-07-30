#ifndef SD_RENDERER_2D_HPP
#define SD_RENDERER_2D_HPP

#include "Core/Export.hpp"
#include <glm/glm.hpp>

namespace sd {

class OrthographicCamera;

class SD_API Renderer2D {
   public:
    static void init();
    static void destory();

    static void startBatch();
    static void flush();
    static void nextBatch();

    static void beginScene(const OrthographicCamera &camera);
    static void endScene();

    static void drawQuad(const glm::mat4 &transform, const glm::vec4 &color);
};

}  // namespace sd

#endif /* SD_RENDERER_2D_HPP */
