#include "ECS/RenderSystem.hpp"
#include "Common/Log.hpp"
#include "ECS/Component.hpp"
#include "Renderer/Renderer3D.hpp"

namespace sd {

RenderSystem::RenderSystem() {
    m_shader = Graphics::assetManager().load<Shader>("shaders/simple3d.glsl");
}

void RenderSystem::setRenderTarget(RenderTarget *target) { m_target = target; }

void RenderSystem::onTick(float) {}

void RenderSystem::setCamera(Camera *camera) { m_camera = camera; }

void RenderSystem::onRender() {
    sd::Renderer3D::beginScene(*m_camera, m_target);
    sd::Renderer::setClearColor(0.1, 0.2, 0.3, 1.0);
    sd::Renderer::clear();
    Framebuffer *framebuffer = m_target->getFramebuffer();
    if (framebuffer) {
        framebuffer->clearAttachment(1, &sd::Entity::INVALID_ID);
    }
    Renderer3D::setShader(*m_shader);

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
    m_shader->bind();
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
    sd::Renderer3D::endScene();
}

}  // namespace sd
