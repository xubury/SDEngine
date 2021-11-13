#ifndef SD_RENDERER_HPP
#define SD_RENDERER_HPP

#include <cstdint>
#include "Renderer/RenderTarget.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Shader.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Font.hpp"
#include "Renderer/Mesh.hpp"
#include "ECS/Scene.hpp"

namespace SD {

class SD_API Renderer {
   public:
    Renderer();
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    Device &device() { return *m_device; }

    void setRenderTarget(RenderTarget &target);

    void updateShader(Shader &shader, Camera &camera);

    void submit(const VertexArray &vao, MeshTopology topology, size_t count,
                size_t offset);

    void drawMesh(const Mesh &mesh);

    void startBatch();
    void flush();
    void nextBatch();

    void beginScene(Camera &camera);
    void endScene();

    void setTextOrigin(float x, float y);

    void drawQuad(const glm::mat4 &transform, const glm::vec4 &color);

    void drawTexture(const Ref<Texture> &texture, const glm::mat4 &transform,
                     const glm::vec4 &color = glm::vec4(1.0f));

    void drawBillboard(const Ref<Texture> &texture, const glm::vec3 &pos,
                       const glm::vec2 &scale,
                       const glm::vec4 &color = glm::vec4(1.0f));

    void drawText(Font &font, const std::wstring &text, int pixleSize,
                  const glm::mat4 &transform,
                  const glm::vec4 &color = glm::vec4(1.0f));

    void setScene(Scene *scene);
    Scene *getScene();

    void setCamera(Camera *camera);
    Camera *getCamera();

   private:
    void initRenderer2D();

    Scene *m_scene;
    Camera *m_camera;

    Ref<UniformBuffer> m_cameraUBO;

    Ref<Device> m_device;
};

}  // namespace SD

#endif /* SD_RENDERER_HPP */
