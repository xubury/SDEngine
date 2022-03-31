#include "Renderer/CameraSystem.hpp"
#include "ECS/Component.hpp"

namespace SD {

CameraSystem::CameraSystem() : ECSSystem("CameraSystem") {}

void CameraSystem::OnRender()
{
    auto view = GetScene().view<CameraComponent, TransformComponent>();
    view.each([](CameraComponent &camComp, TransformComponent &trans) {
        camComp.camera.SetWorldTransform(trans.GetWorldTransform().GetMatrix());
    });
}

}  // namespace SD
