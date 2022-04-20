
#include "ECS/SceneManager.hpp"

namespace SD {

SceneManager::SceneManager() : m_current_index(0) {}

SceneManager::~SceneManager()
{
    for (auto scene : m_scenes) {
        delete scene;
    }
}

void SceneManager::SetCurrentScene(int index) { m_current_index = index; }

Scene *SceneManager::GetCurrentScene() { return m_scenes[m_current_index]; }

}  // namespace SD
