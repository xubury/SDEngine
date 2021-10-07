#include "Core/Layer.hpp"
#include "Core/System.hpp"
#include "ECS/Scene.hpp"

namespace sd {

void Layer::removeSystem(const Ref<System> &system) {
    system->onDestroy();
    auto iter = std::find(m_systems.begin(), m_systems.end(), system);
    if (iter != m_systems.end()) {
        m_systems.erase(iter);
    } else {
        SD_CORE_WARN("System does not exist!");
    }
}

void Layer::setScene(Scene *scene) {
    m_scene = scene;
    SceneEvent event;
    event.scene = scene;
    m_dispatcher.dispatchEvent(event);
}

Scene *Layer::getScene() const { return m_scene; }

}  // namespace sd
