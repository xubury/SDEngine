#ifndef SD_RENDERER_2D_HPP
#define SD_RENDERER_2D_HPP

#include "Utility/Export.hpp"
#include "Utility/Base.hpp"
#include <glm/glm.hpp>

namespace sd {

class OrthographicCamera;

class Texture;

class SD_API Renderer2D {
   public:
    static void init();

    static void startBatch();
    static void flush();
    static void nextBatch();

    static void beginScene(OrthographicCamera &camera);
    static void endScene();

    static void drawQuad(const glm::mat4 &transform, const glm::vec4 &color);

    static void drawTexture(const Ref<Texture> &texture,
                            const glm::mat4 &transform,
                            const glm::vec4 &color = glm::vec4(1.0f));

    static void drawTexture(const Ref<Texture> &texture, const glm::vec3 &pos,
                            const glm::vec3 &scale,
                            const glm::vec4 &color = glm::vec4(1.0f));

    static void drawText(const std::wstring &text, float scale = 1,
                         const glm::vec4 &color = glm::vec4(1.0f));

    static void setTextOrigin(float x, float y);

   private:
    Renderer2D() = default;
};

}  // namespace sd

#endif /* SD_RENDERER_2D_HPP */
