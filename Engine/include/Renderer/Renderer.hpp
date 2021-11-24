#ifndef SD_RENDERER_HPP
#define SD_RENDERER_HPP

#include "Renderer/RenderTarget.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Shader.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Font.hpp"
#include "Renderer/Mesh.hpp"
#include "ECS/Scene.hpp"

namespace SD {

class AssetManager;

class SD_API Renderer {
   public:
    Renderer(AssetManager *manager, int msaa);
    Renderer(const Renderer &) = delete;
    Renderer &operator=(const Renderer &) = delete;

    Device &GetDevice() { return *m_device; }

    void SetRenderTarget(RenderTarget &target);

    void UpdateShader(Shader &shader, Camera &camera);

    void Submit(const VertexArray &vao, MeshTopology topology, size_t count,
                size_t offset);

    void DrawMesh(const Mesh &mesh);

    void StartBatch();
    void Flush();
    void NextBatch();

    void BeginScene(Camera &camera);
    void EndScene();

    void SetTextOrigin(float x, float y);

    void DrawQuad(const glm::mat4 &transform, const glm::vec4 &color);

    void DrawTexture(const Ref<Texture> &texture, const glm::mat4 &transform,
                     const std::array<glm::vec2, 4> &texCoords,
                     const glm::vec4 &color = glm::vec4(1.0f));
    void DrawTexture(const Ref<Texture> &texture, const glm::mat4 &transform,
                     const glm::vec4 &color = glm::vec4(1.0f));

    void DrawBillboard(const Ref<Texture> &texture, const glm::vec3 &pos,
                       const glm::vec2 &scale,
                       const glm::vec4 &color = glm::vec4(1.0f));

    void DrawText(Font &font, const std::string &text, uint8_t pixleSize,
                  const glm::mat4 &transform,
                  const glm::vec4 &color = glm::vec4(1.0f));

    void SetScene(Scene *scene);
    Scene *GetScene();

    void SetCamera(Camera *camera);
    Camera *GetCamera();

   private:
    void InitRenderer2D(AssetManager *manager);

    Scene *m_scene;
    Camera *m_camera;

    Ref<UniformBuffer> m_cameraUBO;

    Ref<Device> m_device;
};

}  // namespace SD

#endif /* SD_RENDERER_HPP */
