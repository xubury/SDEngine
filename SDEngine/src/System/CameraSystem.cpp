#include "System/CameraSystem.hpp"
#include "ECS/Scene.hpp"

namespace SD {

CameraSystem::CameraSystem() : System("CameraSystem") {}

void CameraSystem::OnRender()
{
    auto view = scene->view<CameraComponent, TransformComponent>();
    view.each([](CameraComponent &camComp, TransformComponent &trans) {
        camComp.camera.SetWorldTransform(trans.GetWorldTransform().GetMatrix());
    });
}

}  // namespace SD
