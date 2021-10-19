#ifndef SD_RENDER_SYSTEM_HPP
#define SD_RENDER_SYSTEM_HPP

#include "System/System.hpp"
#include "System/Event.hpp"
#include "ECS/Scene.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/VertexArray.hpp"
#include "Graphics/RenderTarget.hpp"

namespace sd {

enum GeometryBufferType {
    G_POSITION = 0,
    G_NORMAL,
    G_ALBEDO,
    G_AMBIENT,
    G_ENTITY_ID,
    GBUFFER_COUNT
};

class SD_API RenderSystem : public System {
   public:
    RenderSystem(int width, int height, int samples);

    void onInit() override;

    void onDestroy() override;

    void onTick(float dt) override;

    void onRender() override;

    void onSizeEvent(const SizeEvent &event);
    void onSceneEvent(const SceneEvent &event);

    Framebuffer *getGBuffer() const { return m_gBuffer.get(); }

    void setCamera(Camera *camera);
    Camera *getCamera();

    float getExposure() const;
    void setExposure(float exposure);

    bool getBloom() const;
    void setBloom(bool isBloom);

    float getBloomFactor() const;
    void setBloomFactor(float Bloom);

   private:
    void initQuad();
    void initGBuffer(int width, int height, int samples);

    void renderGBuffer();
    void renderShadow();
    void renderLight();

    void renderBlur();

    void renderMain();

    void renderText();

    Ref<Shader> m_mainShader;

    Ref<Shader> m_blurShader;
    RenderTarget m_blurTarget[2];
    Texture *m_blurResult;

    Ref<Shader> m_lightShader;
    RenderTarget m_lightTarget[2];
    RenderTarget *m_lightResult;

    Ref<Shader> m_gBufferShader;
    RenderTarget m_gBufferTarget;
    Ref<Framebuffer> m_gBuffer;

    Ref<Shader> m_shadowShader;

    Ref<VertexArray> m_quad;

    Scene *m_scene;
    Camera *m_camera;

    float m_exposure;

    float m_bloom;
    bool m_isBloom;
};

}  // namespace sd

#endif /* SD_RENDER_SYSTEM_HPP */
