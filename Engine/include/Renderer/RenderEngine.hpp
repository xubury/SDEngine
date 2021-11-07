#ifndef SD_RENDER_ENGINE_HPP
#define SD_RENDER_ENGINE_HPP

#include "Renderer/System/LightingSystem.hpp"
#include "Renderer/System/TerrainSystem.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

class RenderEngine {
   public:
    static void init(int width, int height, int samples);
    static void resize(int width, int height);

    static void render();
    static void postRender();
    static void tick(float dt);

    static API getAPI();

    static TerrainSystem *getTerrainSystem();

    static LightingSystem *getLightingSystem();

    static RenderTarget &getRenderTarget();

    static void updateShader(Shader &shader, Camera &camera);

    static void setScene(Scene *scene);
    static Scene *getScene();

    static void setCamera(Camera *camera);
    static Camera *getCamera();

    static void setExposure(float exposure);
    static float getExposure();

    static void setBloom(bool isBloom);
    static bool getBloom();

    static void setBloomFactor(float bloom);
    static float getBloomFactor();

    static float getGammaCorrection();
    static void setGammaCorrection(float gamma);

    static void renderQuad();

   private:
    static void initSystems();
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
