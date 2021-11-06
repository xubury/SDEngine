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

    static void drawBillboard(const Ref<Texture> &texture, const glm::vec3 &pos,
                              const glm::vec2 &scale,
                              const glm::vec4 &color = glm::vec4(1.0f));

    static void drawText(Font &font, const std::wstring &text, int pixleSize,
                         const glm::mat4 &transform,
                         const glm::vec4 &color = glm::vec4(1.0f));

    static void drawMesh(const Mesh &mesh);
};

}  // namespace sd

#endif /* SD_RENDERER_HPP */
