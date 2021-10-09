#ifndef SD_RENDERER_2D_HPP
#define SD_RENDERER_2D_HPP

#include "Utility/Export.hpp"
#include "Utility/Base.hpp"
#include "Renderer/Renderer.hpp"
#include <glm/glm.hpp>

namespace sd {

class Camera;

class Texture;

class Font;

class SD_API Renderer2D {
   public:
    static void init();

    static void startBatch();
    static void flush();
    static void nextBatch();

    static void beginScene(Camera &camera);
    static void endScene();

    static void drawQuad(const glm::mat4 &transform, const glm::vec4 &color);

    static void drawTexture(const Ref<Texture> &texture,
                            const glm::mat4 &transform,
                            const glm::vec4 &color = glm::vec4(1.0f));

    static void drawText(const Font &font, const std::wstring &text,
                         const glm::vec4 &color = glm::vec4(1.0f),
                         float scale = 1);

    static void setTextOrigin(float x, float y);

   private:
    Renderer2D() = default;
};

}  // namespace sd

#endif /* SD_RENDERER_2D_HPP */
