#include "Renderer/System/ShadowSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "ECS/Component.hpp"
#include "ECS/Scene.hpp"

namespace sd {

ShadowSystem::ShadowSystem() {
    m_shadowShader = ShaderLibrary::instance().load("shaders/shadow.glsl");
}

void ShadowSystem::onInit() {}

void ShadowSystem::onDestroy() {}

void ShadowSystem::onRender() {
    auto scene = Renderer::engine().getScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    Renderer::device().setCullFace(Face::FRONT);
    m_shadowShader->bind();
    lightView.each([this, &modelView](const TransformComponent &transformComp,
                                      LightComponent &lightComp) {
        Light &light = lightComp.light;
        if (!light.isCastShadow()) return;

        light.getRenderTarget().bind();
        light.getRenderTarget().getFramebuffer()->clearDepth();
        light.computeLightSpaceMatrix(transformComp.transform,
                                      Renderer::engine().getCamera());
        m_shadowShader->setMat4("u_projectionView", light.getProjectionView());

        modelView.each([this](const TransformComponent &transformComp,
                              const ModelComponent &modelComp) {
            auto model = AssetManager::instance().get<Model>(modelComp.id);
            m_shadowShader->setMat4(
                "u_model", transformComp.transform.getWorldTransform());
            for (const auto &mesh : model->getMeshes()) {
                Renderer3D::drawMesh(mesh);
            }
        });
    });
    Renderer::device().setCullFace(Face::BACK);
}

}  // namespace sd
