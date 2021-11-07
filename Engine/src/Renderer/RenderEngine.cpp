#include "Renderer/RenderEngine.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/System/ShadowSystem.hpp"
#include "Renderer/System/ProfileSystem.hpp"
#include "Renderer/System/PostProcessSystem.hpp"
#include "Renderer/System/SpriteRenderSystem.hpp"
#include "Graphics/Device.hpp"
#include "System/Event.hpp"
#include "ECS/Scene.hpp"

namespace sd {

struct CameraData {
    glm::mat4 viewProjection;
    glm::vec3 viewPos;
};

struct RenderData {
    int width;
    int height;
    int samples;

    RenderTarget defaultTarget;

    Scene *scene;

    Camera *camera;
    Ref<UniformBuffer> cameraUBO;

    float exposure;

    float bloom;
    bool isBloom;

    SystemManager systems;
    Ref<LightingSystem> lightingSystem;
    Ref<TerrainSystem> terrainSystem;

    Ref<VertexArray> quad;
};

static RenderData s_data;

void RenderEngine::init(int width, int height, int samples) {
    Renderer::init();

    s_data.width = width;
    s_data.height = height;
    s_data.samples = samples;

    s_data.cameraUBO = UniformBuffer::create(nullptr, sizeof(CameraData),
                                             BufferIOType::DYNAMIC);
    s_data.defaultTarget.addTexture(Texture::create(
        width, height, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::EDGE, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR));
    s_data.defaultTarget.addTexture(Texture::create(
        width, height, 1, TextureType::TEX_2D, TextureFormat::DEPTH,
        TextureFormatType::FLOAT, TextureWrap::EDGE, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR));
    s_data.defaultTarget.createFramebuffer();

    s_data.exposure = 1.5;
    s_data.isBloom = true;
    s_data.bloom = 1.0f;

    s_data.systems.addSystem<ShadowSystem>();
    s_data.lightingSystem = s_data.systems.addSystem<LightingSystem>(
        s_data.width, s_data.height, s_data.samples);
    s_data.systems.addSystem<SpriteRenderSystem>();
    s_data.terrainSystem = s_data.systems.addSystem<TerrainSystem>();
    s_data.systems.addSystem<PostProcessSystem>(s_data.width, s_data.height);
    s_data.systems.addSystem<ProfileSystem>(s_data.width, s_data.height);

    const float quadVertices[] = {
        -1.0f, -1.0f, 0.f, 0.f,  0.f,   // bottom left
        1.0f,  -1.0f, 0.f, 1.0f, 0.f,   // bottom right
        1.0f,  1.0f,  0.f, 1.0f, 1.0f,  // top right
        -1.0f, 1.0f,  0.f, 0.f,  1.0f,  // top left
    };
    const uint32_t indices[] = {0, 1, 2, 2, 3, 0};
    auto buffer = VertexBuffer::create(quadVertices, sizeof(quadVertices),
                                       BufferIOType::STATIC);
    auto indexBuffer = IndexBuffer::create(indices, 6, BufferIOType::STATIC);
    s_data.quad = VertexArray::create();
    VertexBufferLayout layout;
    layout.push(BufferDataType::FLOAT, 3);
    layout.push(BufferDataType::FLOAT, 2);
    s_data.quad->addVertexBuffer(buffer, layout);
    s_data.quad->setIndexBuffer(indexBuffer);
}

void RenderEngine::resize(int width, int height) {
    s_data.width = width;
    s_data.height = height;
    if (s_data.camera) {
        s_data.camera->resize(width, height);
    }
    s_data.defaultTarget.resize(width, height);
    s_data.systems.dispatchEvent(SizeEvent(width, height));
}

void RenderEngine::render() {
    Device::instance().setRenderTarget(s_data.defaultTarget);
    Device::instance().clear();
    for (auto &system : s_data.systems.getSystems()) {
        system->onRender();
    }
}

void RenderEngine::postRender() {
    Device::instance().setPolygonMode(PolygonMode::FILL, Face::BOTH);
    Device::instance().blitFramebuffer(
        RenderEngine::getRenderTarget().getFramebuffer(), 0, nullptr, 0,
        BufferBitMask::COLOR_BUFFER_BIT, TextureFilter::LINEAR);
    Device::instance().setFramebuffer(nullptr);
}

void RenderEngine::tick(float dt) {
    for (auto &system : s_data.systems.getSystems()) {
        system->onTick(dt);
    }
}

TerrainSystem *RenderEngine::getTerrainSystem() {
    return s_data.terrainSystem.get();
}

LightingSystem *RenderEngine::getLightingSystem() {
    return s_data.lightingSystem.get();
}

RenderTarget &RenderEngine::getRenderTarget() { return s_data.defaultTarget; }

void RenderEngine::updateShader(Shader &shader, Camera &camera) {
    CameraData cameraData;
    cameraData.viewPos = camera.getWorldPosition();
    cameraData.viewProjection = camera.getViewPorjection();
    s_data.cameraUBO->updateData(&cameraData, sizeof(CameraData));
    shader.setUniformBuffer("Camera", *s_data.cameraUBO);
}

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

void RenderEngine::renderQuad() {
    Device::instance().submit(*s_data.quad, MeshTopology::TRIANGLES,
                              s_data.quad->getIndexBuffer()->getCount(), 0);
}

}  // namespace sd
