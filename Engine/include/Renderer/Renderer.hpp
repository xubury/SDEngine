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
#include "Renderer/System/LightingSystem.hpp"
#include "Renderer/System/TerrainSystem.hpp"

namespace sd {

class SD_API Renderer {
   public:
    static Renderer &engine();
    static Device &device();

    void init(int width, int height, int samples);
    void resize(int width, int height);

    void render();
    void postRender();
    void tick(float dt);

    TerrainSystem *getTerrainSystem();

    LightingSystem *getLightingSystem();

    RenderTarget &getRenderTarget();

    void updateShader(Shader &shader, Camera &camera);

    void setScene(Scene *scene);
    Scene *getScene();

    void setCamera(Camera *camera);
    Camera *getCamera();

    void setExposure(float exposure);
    float getExposure();

    void setBloom(bool isBloom);
    bool getBloom();

    void setBloomFactor(float bloom);
    float getBloomFactor();

    float getGammaCorrection();
    void setGammaCorrection(float gamma);

    void submit(const VertexArray &vao, MeshTopology topology, size_t count,
                size_t offset);

   private:
    void initSystems();

    int m_width;
    int m_height;
    int m_samples;

    RenderTarget m_defaultTarget;
    Scene *m_scene;

    Camera *m_camera;
    float m_exposure;
    float m_bloom;
    bool m_isBloom;
    float m_gammaCorrection;

    Ref<UniformBuffer> m_cameraUBO;

    Ref<Device> m_device;
    SystemManager m_systems;
    Ref<LightingSystem> m_lightingSystem;
    Ref<TerrainSystem> m_terrainSystem;
};

class SD_API Renderer3D {
   public:
    static void drawMesh(const Mesh &mesh);
};

class SD_API Renderer2D {
    friend class Renderer;

   private:
    static void init();

   public:
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
};

}  // namespace sd

#endif /* SD_RENDERER_HPP */
