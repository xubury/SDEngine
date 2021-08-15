#include "ECS/RenderSystem.hpp"
#include "Common/Log.hpp"
#include "ECS/Component.hpp"
#include "Renderer/Renderer3D.hpp"

namespace sd {

RenderSystem::RenderSystem(Scene *scene) : m_scene(scene) {}

void RenderSystem::setScene(Scene *scene) { m_scene = scene; }

void RenderSystem::onTick(float) {}

void RenderSystem::onRender() {
    auto view = m_scene->getRegistry()
                    .view<sd::TransformComponent, sd::ModelComponent>();
    view.each([](const entt::entity &entity,
                 const TransformComponent &transformComp,
                 const ModelComponent &modelComp) {
        for (const auto &mesh : modelComp.model->getMeshes()) {
            Renderer3D::drawMesh(mesh, transformComp.transform,
                                 static_cast<uint32_t>(entity));
        }
    });
}

}  // namespace sd
