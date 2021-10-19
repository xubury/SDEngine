#ifndef SD_RENDERER_HPP
#define SD_RENDERER_HPP

#include <cstdint>
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Font.hpp"
#include "Graphics/Mesh.hpp"

namespace sd {

class SD_API Renderer {
   public:
    static void init();

    static void submit(const VertexArray &vao, MeshTopology topology,
                       size_t count, size_t offset);

    static void setRenderTarget(const RenderTarget &target);

    static void startBatch();
    static void flush();
    static void nextBatch();

    static void beginScene(Camera &camera);
    static void endScene();

    static void setTextOrigin(float x, float y);

    static void drawQuad(const glm::mat4 &transform, const glm::vec4 &color);

    static void drawTexture(const Ref<Texture> &texture,
                            const glm::mat4 &transform,
                            const glm::vec4 &color = glm::vec4(1.0f));

    static void drawText(const Font &font, const std::wstring &text,
                         const glm::mat4 &transform,
                         const glm::vec4 &color = glm::vec4(1.0f));

    static void drawText(const Font &font, const std::wstring &text,
                         float pixelScale = 10,
                         const glm::vec4 &color = glm::vec4(1.0f));

    static void drawMesh(const Mesh &mesh);

    static void setShader(Shader &shader);
};

}  // namespace sd

#endif /* SD_RENDERER_HPP */
