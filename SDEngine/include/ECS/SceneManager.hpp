#ifndef SD_SCENE_MANAGER_HPP
#define SD_SCENE_MANAGER_HPP

#include "ECS/Export.hpp"
#include "ECS/Scene.hpp"
#include <vector>
#include <utility>

namespace SD {

class SD_ECS_API SceneManager {
    using Container = std::vector<Scene *>;

   public:
    SceneManager();
    virtual ~SceneManager();
    SceneManager(const SceneManager &) = delete;
    SceneManager &operator=(const SceneManager &) = delete;

    template <typename... Args>
    void EmplaceScene(Args &&...args)
    {
        m_scenes.emplace_back(new Scene(std::forward<Args>(args)...));
    }

    void SetCurrentScene(int index);
    Scene *GetCurrentScene();
    Scene *GetScene(int index) { return m_scenes[index]; }
    bool IsCurrentScene(int index) { return m_current_index == index; }

    Container::iterator begin() { return m_scenes.begin(); }

    Container::iterator end() { return m_scenes.end(); }

    Container::reverse_iterator rbegin() { return m_scenes.rbegin(); }

    Container::reverse_iterator rend() { return m_scenes.rend(); }

    Container::const_iterator cbegin() const { return m_scenes.cbegin(); }

    Container::const_iterator end() const { return m_scenes.end(); }

    Container::const_reverse_iterator rbegin() const
    {
        return m_scenes.rbegin();
    }

    Container::const_reverse_iterator rend() const { return m_scenes.rend(); }

    size_t Size() const { return m_scenes.size(); }

   private:
    int m_current_index;
    Container m_scenes;
};

}  // namespace SD

#endif
