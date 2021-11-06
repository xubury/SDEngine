#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/ShadowSystem.hpp"
#include "Renderer/RenderSystem.hpp"
#include "Renderer/ProfileSystem.hpp"
#include "Renderer/TerrainSystem.hpp"
#include "Graphics/Camera.hpp"
#include "Graphics/Device.hpp"
#include "System/Event.hpp"
#include "ECS/Scene.hpp"

namespace sd {

struct RenderData {
    int width;
    int height;
    int samples;

    RenderTarget target;

    Scene *scene;
    Camera *camera;

    float exposure;

    float bloom;
    bool isBloom;

    SystemManager systems;
    Ref<RenderSystem> renderSystem;
    Ref<TerrainSystem> terrainSystem;
};

static RenderData s_data;

void RenderEngine::init(int width, int height, int samples) {
    Renderer::init();

    s_data.width = width;
    s_data.height = height;
    s_data.samples = samples;

    s_data.exposure = 1.5;
    s_data.isBloom = true;
    s_data.bloom = 1.0f;

    s_data.systems.addSystem<ShadowSystem>();
    s_data.renderSystem = s_data.systems.addSystem<RenderSystem>(
        s_data.width, s_data.height, s_data.samples);
    s_data.systems.addSystem<ProfileSystem>(s_data.width, s_data.height);
    s_data.terrainSystem = s_data.systems.addSystem<TerrainSystem>();
}

void RenderEngine::resize(int width, int height) {
    s_data.width = width;
    s_data.height = height;
    if (s_data.camera) {
        s_data.camera->resize(width, height);
    }
    s_data.camera->resize(width, height);
    s_data.target.resize(width, height);
    s_data.systems.dispatchEvent(SizeEvent(width, height));
}

void RenderEngine::render() {
    for (auto &system : s_data.systems.getSystems()) {
        system->onRender();
    }
}

void RenderEngine::tick(float dt) {
    for (auto &system : s_data.systems.getSystems()) {
        system->onTick(dt);
    }
}

RenderSystem *RenderEngine::getRenderSystem() {
    return s_data.renderSystem.get();
}

TerrainSystem *RenderEngine::getTerrainSystem() {
    return s_data.terrainSystem.get();
}

const RenderTarget &RenderEngine::getRenderTarget() { return s_data.target; }

void RenderEngine::setScene(Scene *scene) { s_data.scene = scene; }
Scene *RenderEngine::getScene() { return s_data.scene; }

void RenderEngine::setCamera(Camera *camera) { s_data.camera = camera; }
Camera *RenderEngine::getCamera() { return s_data.camera; }

void RenderEngine::setExposure(float exposure) { s_data.exposure = exposure; }
float RenderEngine::getExposure() { return s_data.exposure; }

void RenderEngine::setBloom(bool isBloom) { s_data.isBloom = isBloom; }
bool RenderEngine::getBloom() { return s_data.isBloom; }

void RenderEngine::setBloomFactor(float bloom) { s_data.bloom = bloom; }
float RenderEngine::getBloomFactor() { return s_data.bloom; }

}  // namespace sd
