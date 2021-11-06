#ifndef SD_RENDER_ENGINE_HPP
#define SD_RENDER_ENGINE_HPP

#include "Renderer/RenderSystem.hpp"
#include "Renderer/TerrainSystem.hpp"
#include "Graphics/Camera.hpp"

namespace sd {

class RenderEngine {
   public:
    static void init(int width, int height, int samples);
    static void resize(int width, int height);

    static void render();
    static void tick(float dt);

    static API getAPI();

    static RenderSystem *getRenderSystem();
    static TerrainSystem *getTerrainSystem();

    static const RenderTarget &getRenderTarget();

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
};

}  // namespace sd

#endif /* SD_RENDER_ENGINE_HPP */
