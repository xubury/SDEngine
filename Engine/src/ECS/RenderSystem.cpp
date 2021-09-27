#include "ECS/RenderSystem.hpp"
#include "Graphics/Device.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Renderer/RenderEngine.hpp"
#include "Utils/Log.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Component.hpp"

namespace sd {

static Ref<Texture> createGeometryTexture(int width, int height, int samples) {
    return Texture::create(
        width, height, samples,
        samples > 1 ? TextureType::TEX_2D_MULTISAMPLE : TextureType::TEX_2D,
        TextureFormat::RGBA, TextureFormatType::FLOAT, TextureWrap::BORDER,
        TextureFilter::LINEAR, TextureMipmapFilter::LINEAR_NEAREST);
}

RenderSystem::RenderSystem(RenderEngine *engine, int width, int height,
                           int samples)
    : m_engine(engine) {
    Device::instance().setMultisample(samples > 1);
    m_shader = Asset::manager().load<Shader>("shaders/main3d.glsl");
    m_framebuffer = Framebuffer::create();
    m_gbufferShader = Asset::manager().load<Shader>("shaders/gbuffer.glsl");
    for (int i = 0; i < 4; ++i) {
        m_gBufferTarget.addTexture(createGeometryTexture(width, height, 8));
        m_framebuffer->attachTexture(createGeometryTexture(width, height, 1));
    }
    // entity id
    m_gBufferTarget.addTexture(Texture::create(
        width, height, samples, TextureType::TEX_2D_MULTISAMPLE,
        TextureFormat::ALPHA, TextureFormatType::UINT, TextureWrap::BORDER,
        TextureFilter::NEAREST, TextureMipmapFilter::NEAREST));
    // depth
    m_gBufferTarget.addTexture(Texture::create(
        width, height, samples, TextureType::TEX_2D_MULTISAMPLE,
        TextureFormat::DEPTH, TextureFormatType::FLOAT, TextureWrap::BORDER,
        TextureFilter::LINEAR, TextureMipmapFilter::LINEAR_NEAREST));
    m_gBufferTarget.init();
    // entity id
    m_framebuffer->attachTexture(
        Texture::create(800, 600, 1, TextureType::TEX_2D, TextureFormat::ALPHA,
                        TextureFormatType::UINT, TextureWrap::BORDER,
                        TextureFilter::NEAREST, TextureMipmapFilter::NEAREST));

    float quadVertices[] = {
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f,  // top right
        1.0f,  -1.0f, 0.0f, 1.0f, 0.f,   // bottom right
        -1.0f, -1.0f, 0.0f, 0.f,  0.f,   // bottom left
        -1.0f, 1.0f,  0.0f, 0.f,  1.0f   // top left
    };
    unsigned int indices[] = {0, 1, 3, 1, 2, 3};
    Ref<VertexBuffer> buffer = VertexBuffer::create(
        quadVertices, sizeof(quadVertices), BufferIOType::STATIC);
    Ref<IndexBuffer> indexBuffer =
        IndexBuffer::create(indices, 6, BufferIOType::STATIC);
    m_vao = VertexArray::create();
    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(2);
    m_vao->addVertexBuffer(buffer, layout);
    m_vao->setIndexBuffer(indexBuffer);
}

void RenderSystem::resize(int width, int height) {
    m_gBufferTarget.resize(width, height);
    m_framebuffer->resize(width, height);
}

void RenderSystem::onTick(float) {}

void RenderSystem::onRender() {
    renderGBuffer();
    renderMain();
}

void RenderSystem::renderMain() {
    Renderer3D::beginScene(*m_engine->getCamera(), m_engine->getRenderTarget());
    Renderer::setShader(*m_shader);
    Device::instance().setClearColor(0.1, 0.2, 0.3, 1.0);
    Device::instance().clear();
    Device::instance().setDepthMask(false);
    auto lightView = m_scene->view<TransformComponent, LightComponent>();
    lightView.each([this](const TransformComponent &transformComp,
                          const LightComponent &lightComp) {
        const Transform &transform = transformComp.transform;
        const Light &light = lightComp.light;
        m_shader->setVec3("u_light.direction", transform.getWorldFront());
        m_shader->setVec3("u_light.ambient", light.ambient);
        m_shader->setVec3("u_light.diffuse", light.diffuse);
        m_shader->setVec3("u_light.specular", light.specular);
        m_shader->setVec3("u_light.position", transform.getWorldPosition());
        m_shader->setFloat("u_light.isDirectional", light.isDirectional);
        m_shader->setFloat("u_light.cutOff",
                           glm::cos(glm::radians(light.cutOff)));
        m_shader->setFloat("u_light.outerCutOff",
                           glm::cos(glm::radians(light.outerCutOff)));
        m_shader->setFloat("u_light.constant", light.constant);
        m_shader->setFloat("u_light.linear", light.linear);
        m_shader->setFloat("u_light.quadratic", light.quadratic);
    });

    Framebuffer *gBuffer = getGBuffer();
    m_shader->setTexture("u_position", gBuffer->getTexture(0));
    m_shader->setTexture("u_normal", gBuffer->getTexture(1));
    m_shader->setTexture("u_albedo", gBuffer->getTexture(2));
    m_shader->setTexture("u_ambient", gBuffer->getTexture(3));
    m_shader->setTexture("u_entityTexture", gBuffer->getTexture(4));
    m_shader->setFloat("u_exposure", m_engine->getExposure());
    Renderer::submit(*m_vao.get(), MeshTopology::TRIANGLES, 6, 0);
    Device::instance().setDepthMask(true);
    Renderer3D::endScene();
}

void RenderSystem::renderGBuffer() {
    Renderer3D::beginScene(*m_engine->getCamera(), &m_gBufferTarget);
    Device::instance().setBlend(false);
    Device::instance().setClearColor(0.f, 0.f, 0.f, 1.0);
    Device::instance().clear();
    Renderer::setShader(*m_gbufferShader);
    m_gBufferTarget.getFramebuffer()->clearAttachment(4, &Entity::INVALID_ID);
    auto modelView = m_scene->view<TransformComponent, ModelComponent>();
    modelView.each([this](const entt::entity &entity,
                          const TransformComponent &transformComp,
                          const ModelComponent &modelComp) {
        m_gbufferShader->setMat4("u_world",
                                 transformComp.transform.getWorldTransform());
        m_gbufferShader->setUint("u_entityId", static_cast<uint32_t>(entity));
        m_gbufferShader->setVec3("u_color", modelComp.color);
        auto &material = modelComp.model->getMaterial();
        m_gbufferShader->setTexture("u_material.diffuse",
                                    material.getTexture(MaterialType::DIFFUSE));
        m_gbufferShader->setTexture(
            "u_material.specular", material.getTexture(MaterialType::SPECULAR));
        m_gbufferShader->setTexture("u_material.ambient",
                                    material.getTexture(MaterialType::AMBIENT));
        for (const auto &mesh : modelComp.model->getMeshes()) {
            Renderer3D::drawMesh(mesh);
        }
    });
    m_framebuffer->copyFrom(m_gBufferTarget.getFramebuffer(),
                            BufferBit::COLOR_BUFFER_BIT,
                            TextureFilter::NEAREST);
    Device::instance().setBlend(true);
    Renderer3D::endScene();
}

Framebuffer *RenderSystem::getGBuffer() { return m_framebuffer.get(); }

}  // namespace sd
