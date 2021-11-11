#ifndef SD_RENDERER_HPP
#define SD_RENDERER_HPP

#include <cstdint>
#include "Core/Layer.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Graphics/Graphics.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Shader.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Font.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/System/LightingSystem.hpp"
#include "Renderer/System/TerrainSystem.hpp"

namespace SD {

namespace Renderer {

class SD_API Engine : public SystemManager, public Layer {
   public:
    void init(int width, int height, int samples);

    void onRender() override;
    void onTick(float dt) override;
    void onEventProcess(const SDL_Event &event) override;
    void onEventsProcess() override;

    void resize(int width, int height);

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

   private:
    friend Engine &engine();
    friend Device &device();
    Engine();
    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;

    void initSystems();
    void initRenderer2D();

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

    Ref<LightingSystem> m_lightingSystem;
    Ref<TerrainSystem> m_terrainSystem;

    Ref<Device> m_device;
};

Engine &engine();
Device &device();

void submit(const VertexArray &vao, MeshTopology topology, size_t count,
            size_t offset);

};  // namespace Renderer

namespace Renderer3D {

void drawMesh(const Mesh &mesh);

};  // namespace Renderer3D

namespace Renderer2D {

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

};  // namespace Renderer2D

}  // namespace SD

#endif /* SD_RENDERER_HPP */
