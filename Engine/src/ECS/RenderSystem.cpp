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
    // entity id
    m_gBufferTarget.addTexture(Texture::create(
        800, 600, 8, TextureType::TEX_2D_MULTISAMPLE, TextureFormat::ALPHA,
        TextureFormatType::UINT, TextureWrap::BORDER, TextureFilter::NEAREST,
        TextureMipmapFilter::NEAREST));
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
    Renderer3D::setShader(*m_shader);
    Renderer::setDepthMask(false);
    m_shader->bind();
    auto lightView =
        m_scene->getRegistry().view<TransformComponent, LightComponent>();
    lightView.each([this](const TransformComponent &transformComp,
                          const LightComponent &lightComp) {
        m_shader->setVec3("u_light.direction",
                          transformComp.transform.getWorldFront());
        m_shader->setVec3("u_light.ambient", lightComp.light.ambient);
        m_shader->setVec3("u_light.diffuse", lightComp.light.diffuse);
        m_shader->setVec3("u_light.specular", lightComp.light.specular);
    });

    Framebuffer *gBuffer = getGBuffer();
    m_shader->setTexture("u_position", gBuffer->getTexture(0));
    m_shader->setTexture("u_normal", gBuffer->getTexture(1));
    m_shader->setTexture("u_albedo", gBuffer->getTexture(2));
    m_shader->setTexture("u_ambient", gBuffer->getTexture(3));
    m_shader->setTexture("u_entityTexture", gBuffer->getTexture(4));
    Renderer::submit(*m_vao.get(), MeshTopology::TRIANGLES, 6, 0);
    Renderer::setDepthMask(true);
    Renderer3D::endScene();
}

void RenderSystem::renderGBuffer() {
    Renderer::setBlend(false);
    Renderer3D::beginScene(*m_camera, &m_gBufferTarget);
    Renderer::setClearColor(0.f, 0.f, 0.f, 1.0);
    Renderer::clear();
    Renderer3D::setShader(*m_gbufferShader);
    m_gbufferShader->bind();
    m_gBufferTarget.getFramebuffer()->clearAttachment(4, &Entity::INVALID_ID);
    auto modelView =
        m_scene->getRegistry().view<TransformComponent, ModelComponent>();
    modelView.each([this](const entt::entity &entity,
                          const TransformComponent &transformComp,
                          const ModelComponent &modelComp) {
        m_gbufferShader->setMat4("u_world",
                                 transformComp.transform.getWorldTransform());
        m_gbufferShader->setUint("u_entityId", static_cast<uint32_t>(entity));
        for (const auto &material : modelComp.model->getMaterials()) {
            m_gbufferShader->setTexture(
                "u_material.diffuse",
                material.getTexture(MaterialType::DIFFUSE));
            m_gbufferShader->setTexture(
                "u_material.specular",
                material.getTexture(MaterialType::SPECULAR));
            m_gbufferShader->setTexture(
                "u_material.ambient",
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
    Renderer::setBlend(true);
}

Framebuffer *RenderSystem::getGBuffer() { return m_framebuffer.get(); }

}  // namespace sd
