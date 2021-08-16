#include "ECS/RenderSystem.hpp"
#include "Common/Log.hpp"
#include "ECS/Component.hpp"
#include "Renderer/Renderer3D.hpp"

namespace sd {

RenderSystem::RenderSystem(Scene *scene) : m_scene(scene) {
    m_shader = Graphics::assetManager().load<Shader>("shaders/simple3d.glsl");
}

void RenderSystem::setScene(Scene *scene) { m_scene = scene; }

void RenderSystem::onTick(float) {}

void RenderSystem::onRender() {
    auto view = m_scene->getRegistry()
                    .view<sd::TransformComponent, sd::ModelComponent>();
    m_shader->bind();
    Renderer3D::setShader(*m_shader);
    view.each([this](const entt::entity &entity,
                     const TransformComponent &transformComp,
                     const ModelComponent &modelComp) {
        for (const auto &material : modelComp.model->getMaterials()) {
            m_shader->setTexture(
                "u_material.diffuse",
                material.getTexture(MaterialType::DIFFUSE).get());
            m_shader->setTexture(
                "u_material.specular",
                material.getTexture(MaterialType::SPECULAR).get());
            m_shader->setTexture(
                "u_material.ambient",
                material.getTexture(MaterialType::AMBIENT).get());
        }
        m_shader->setMat4("u_world",
                          transformComp.transform.getWorldTransform());
        m_shader->setUint("u_entityId", static_cast<uint32_t>(entity));
        for (const auto &mesh : modelComp.model->getMeshes()) {
            Renderer3D::drawMesh(mesh);
        }
    });
}

}  // namespace sd
