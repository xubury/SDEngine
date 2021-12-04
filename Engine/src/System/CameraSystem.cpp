#include "System/CameraSystem.hpp"
#include "ECS/Scene.hpp"

namespace SD {

CameraSystem::CameraSystem() : System("CameraSystem") {}

void CameraSystem::OnRender() {
    auto scene = renderer->GetScene();
    auto view = scene->view<CameraComponent, TransformComponent>();
    view.each([](CameraComponent &camComp, TransformComponent &transComp) {
        camComp.camera.SetWorldTransform(
            transComp.transform.GetWorldTransform());
    });
}

}  // namespace SD
