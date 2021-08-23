#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include <set>
#include "Common/Base.hpp"
#include "ECS/Scene.hpp"

namespace sd {

class System {
   public:
    System() = default;
    virtual ~System() = default;

    virtual void onTick(float dt) = 0;

    virtual void onRender() = 0;

    virtual void onSceneChange() = 0;

   protected:
    friend class SystemManager;
    Scene *m_scene;
};

class SystemManager {
   public:
    static SystemManager &instance();

    template <typename SYSTEM, typename... ARGS>
    SYSTEM *addSystem(ARGS... args);

    void removeSystem(const Ref<System> &system);

    void setScene(Scene *scene);

   private:
    friend class Application;

    void tick(float dt);

    void render();

    SystemManager() = default;

    std::set<Ref<System>> m_systems;
};

template <typename SYSTEM, typename... ARGS>
SYSTEM *SystemManager::addSystem(ARGS... args) {
    Ref<SYSTEM> system = createRef<SYSTEM>(std::forward<ARGS>(args)...);
    m_systems.emplace(system);
    return system.get();
}

}  // namespace sd

#endif /* SD_SYSTEM_HPP */
