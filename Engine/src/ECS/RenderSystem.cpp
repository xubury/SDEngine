#include "ECS/RenderSystem.hpp"
#include "Common/Log.hpp"
#include "ECS/Component.hpp"
#include "Renderer/Renderer3D.hpp"

namespace sd {

RenderSystem::RenderSystem() {
    m_shader = Graphics::assetManager().load<Shader>("shaders/simple3d.glsl");
    m_gbufferShader =
        Graphics::assetManager().load<Shader>("shaders/gbuffer.glsl");
    // position
    m_gBufferTarget.addTexture(Texture::create(
        800, 600, 8, TextureType::TEX_2D_MULTISAMPLE, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    // normal
    m_gBufferTarget.addTexture(Texture::create(
        800, 600, 8, TextureType::TEX_2D_MULTISAMPLE, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    // albedo
    m_gBufferTarget.addTexture(Texture::create(
        800, 600, 8, TextureType::TEX_2D_MULTISAMPLE, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    // ambient
    m_gBufferTarget.addTexture(Texture::create(
        800, 600, 8, TextureType::TEX_2D_MULTISAMPLE, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    // depth
    m_gBufferTarget.addTexture(Texture::create(
        800, 600, 8, TextureType::TEX_2D_MULTISAMPLE, TextureFormat::DEPTH,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    m_gBufferTarget.init();

    m_framebuffer = Framebuffer::create();
    // position
    m_framebuffer->attachTexture(Texture::create(
        800, 600, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    // normal
    m_framebuffer->attachTexture(Texture::create(
        800, 600, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    // albedo
    m_framebuffer->attachTexture(Texture::create(
        800, 600, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
    // ambient
    m_framebuffer->attachTexture(Texture::create(
        800, 600, 1, TextureType::TEX_2D, TextureFormat::RGBA,
        TextureFormatType::FLOAT, TextureWrap::BORDER, TextureFilter::LINEAR,
        TextureMipmapFilter::LINEAR_NEAREST));
}

void RenderSystem::setRenderTarget(RenderTarget *target) {
    m_gBufferTarget.resize(target->getWidth(), target->getHeight());
    m_framebuffer->resize(target->getWidth(), target->getHeight());
    m_target = target;
}

void RenderSystem::onTick(float) {}

void RenderSystem::setCamera(Camera *camera) { m_camera = camera; }

void RenderSystem::onRender() {
    renderGBuffer();

    Renderer3D::beginScene(*m_camera, m_target);
    Renderer::setClearColor(0.1, 0.2, 0.3, 1.0);
    Renderer::clear();
    Framebuffer *framebuffer = m_target->getFramebuffer();
    if (framebuffer) {
        framebuffer->clearAttachment(1, &Entity::INVALID_ID);
    }
    Renderer3D::setShader(*m_shader);
    m_shader->bind();

    auto terrainView =
        m_scene->getRegistry().view<TransformComponent, TerrainComponent>();
    terrainView.each([this](const entt::entity &entity,
                            const TransformComponent &transformComp,
                            const TerrainComponent &terrainComp) {
        const Material &material = terrainComp.terrain.getMaterial();
        m_shader->setTexture("u_material.diffuse",
                             material.getTexture(MaterialType::DIFFUSE));
        m_shader->setTexture("u_material.specular",
                             material.getTexture(MaterialType::SPECULAR));
        m_shader->setTexture("u_material.ambient",
                             material.getTexture(MaterialType::AMBIENT));
        m_shader->setMat4("u_world",
                          transformComp.transform.getWorldTransform());
        m_shader->setUint("u_entityId", static_cast<uint32_t>(entity));
        Renderer3D::drawMesh(terrainComp.terrain.getMesh());
    });

    auto modelView =
        m_scene->getRegistry().view<TransformComponent, ModelComponent>();
    modelView.each([this](const entt::entity &entity,
                          const TransformComponent &transformComp,
                          const ModelComponent &modelComp) {
        for (const auto &material : modelComp.model->getMaterials()) {
            m_shader->setTexture("u_material.diffuse",
                                 material.getTexture(MaterialType::DIFFUSE));
            m_shader->setTexture("u_material.specular",
                                 material.getTexture(MaterialType::SPECULAR));
            m_shader->setTexture("u_material.ambient",
                                 material.getTexture(MaterialType::AMBIENT));
        }
        m_shader->setMat4("u_world",
                          transformComp.transform.getWorldTransform());
        m_shader->setUint("u_entityId", static_cast<uint32_t>(entity));
        for (const auto &mesh : modelComp.model->getMeshes()) {
            Renderer3D::drawMesh(mesh);
        }
    });
    Renderer3D::endScene();
}

void RenderSystem::renderGBuffer() {
    Renderer3D::beginScene(*m_camera, &m_gBufferTarget);
    Renderer::setClearColor(0.1, 0.2, 0.3, 1.0);
    Renderer::clear();
    Renderer3D::setShader(*m_gbufferShader);
    m_gbufferShader->bind();
    auto modelView =
        m_scene->getRegistry().view<TransformComponent, ModelComponent>();
    modelView.each([this](const TransformComponent &transformComp,
                          const ModelComponent &modelComp) {
        m_gbufferShader->setMat4("u_world",
                                 transformComp.transform.getWorldTransform());
        for (const auto &material : modelComp.model->getMaterials()) {
            m_gbufferShader->setTexture("u_material.diffuse",
                                 material.getTexture(MaterialType::DIFFUSE));
            m_gbufferShader->setTexture("u_material.specular",
                                 material.getTexture(MaterialType::SPECULAR));
            m_gbufferShader->setTexture("u_material.ambient",
                                 material.getTexture(MaterialType::AMBIENT));
        }
        for (const auto &mesh : modelComp.model->getMeshes()) {
            Renderer3D::drawMesh(mesh);
        }
    });
    m_framebuffer->copyFrom(m_gBufferTarget.getFramebuffer(),
                            BufferBit::COLOR_BUFFER_BIT,
                            TextureFilter::NEAREST);
    Renderer3D::endScene();
}

Framebuffer *RenderSystem::getGBuffer() { return m_framebuffer.get(); }

}  // namespace sd
