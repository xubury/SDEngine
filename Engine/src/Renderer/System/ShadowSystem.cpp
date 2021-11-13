#include "Renderer/System/ShadowSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "ECS/Component.hpp"
#include "ECS/Scene.hpp"

namespace SD {

ShadowSystem::ShadowSystem() : System("Shadow") {
    m_shadowShader = ShaderLibrary::instance().load("shaders/shadow.glsl");
}

void ShadowSystem::onInit() {}

void ShadowSystem::onDestroy() {}

void ShadowSystem::onRender() {
    auto scene = renderer->getScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    renderer->device().setCullFace(Face::FRONT);
    m_shadowShader->bind();
    lightView.each([this, &modelView](const TransformComponent &transformComp,
                                      LightComponent &lightComp) {
        Light &light = lightComp.light;
        if (!light.isCastShadow()) return;

        renderer->setRenderTarget(light.getRenderTarget());
        light.getRenderTarget().getFramebuffer()->clearDepth();
        light.computeLightSpaceMatrix(transformComp.transform,
                                      renderer->getCamera());
        m_shadowShader->setMat4("u_projectionView", light.getProjectionView());

        modelView.each([this](const TransformComponent &transformComp,
                              const ModelComponent &modelComp) {
            auto model = AssetManager::instance().get<Model>(modelComp.id);
            m_shadowShader->setMat4(
                "u_model", transformComp.transform.getWorldTransform());
            if (model) {
                for (const auto &mesh : model->getMeshes()) {
                    renderer->drawMesh(mesh);
                }
            }
        });
    });
    renderer->device().setCullFace(Face::BACK);
}

}  // namespace SD
