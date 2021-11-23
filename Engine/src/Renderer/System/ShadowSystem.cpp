#include "Renderer/System/ShadowSystem.hpp"
#include "Renderer/Renderer.hpp"
#include "Asset/Asset.hpp"
#include "ECS/Component.hpp"
#include "ECS/Scene.hpp"

namespace SD {

ShadowSystem::ShadowSystem() : System("Shadow") {}

void ShadowSystem::OnInit() {
    m_shadowShader = asset->LoadAndGet<Shader>("shaders/shadow.glsl");
}

void ShadowSystem::OnPush() {}

void ShadowSystem::OnPop() {}

void ShadowSystem::OnRender() {
    auto scene = renderer->GetScene();
    auto lightView = scene->view<TransformComponent, LightComponent>();
    auto modelView = scene->view<TransformComponent, ModelComponent>();

    renderer->GetDevice().SetCullFace(Face::FRONT);
    m_shadowShader->Bind();
    lightView.each([this, &modelView](const TransformComponent &transformComp,
                                      LightComponent &lightComp) {
        Light &light = lightComp.light;
        if (!light.IsCastShadow()) return;

        renderer->SetRenderTarget(light.GetRenderTarget());
        light.GetRenderTarget().GetFramebuffer()->ClearDepth();
        light.ComputeLightSpaceMatrix(transformComp.transform,
                                      renderer->GetCamera());
        m_shadowShader->SetMat4("u_projectionView", light.GetProjectionView());

        modelView.each([this](const TransformComponent &transformComp,
                              const ModelComponent &modelComp) {
            auto model = asset->Get<Model>(modelComp.id);
            m_shadowShader->SetMat4(
                "u_model", transformComp.transform.GetWorldTransform());
            if (model) {
                for (const auto &mesh : model->GetMeshes()) {
                    renderer->DrawMesh(mesh);
                }
            }
        });
    });
    renderer->GetDevice().SetCullFace(Face::BACK);
}

}  // namespace SD
