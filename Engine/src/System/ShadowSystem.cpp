#include "System/ShadowSystem.hpp"
#include "Graphics/Device.hpp"
#include "Graphics/Renderer.hpp"
#include "Core/Application.hpp"
#include "ECS/Component.hpp"

namespace sd {

ShadowSystem::ShadowSystem() {
    m_shadowShader = Asset::manager().load<Shader>("shaders/shadow.glsl");
}

void ShadowSystem::onTick(float) {}

void ShadowSystem::onRender() {
    auto scene = Application::getRenderEngine().getScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    Device::instance().setCullFace(Face::FRONT);
    m_shadowShader->bind();
    lightView.each([this, &modelView](const TransformComponent &transformComp,
                                      LightComponent &lightComp) {
        Light &light = lightComp.light;
        if (!light.isCastShadow()) return;

        Renderer::setRenderTarget(light.getRenderTarget());
        Device::instance().clear(BufferBitMask::DEPTH_BUFFER_BIT);
        light.computeLightSpaceMatrix(
            transformComp.transform,
            Application::getRenderEngine().getCamera());
        m_shadowShader->setMat4("u_projectionView", light.getProjectionView());

        modelView.each([this](const TransformComponent &transformComp,
                              const ModelComponent &modelComp) {
            m_shadowShader->setMat4(
                "u_model", transformComp.transform.getWorldTransform());
            for (const auto &mesh : modelComp.model->getMeshes()) {
                Renderer::drawMesh(mesh);
            }
        });
    });
    Device::instance().setCullFace(Face::BACK);
}

}  // namespace sd
