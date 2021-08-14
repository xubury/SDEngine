#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include "Common/Base.hpp"
#include "ECS/Scene.hpp"

namespace sd {

class System {
   public:
    System() = default;
    virtual ~System() = default;

   protected:
    friend class SystemManager;

    virtual void onTick(float dt) = 0;

    virtual void onRender() = 0;
};

class SystemManager {
   public:
    SystemManager() = default;

    void addSystem(Ref<System> system);

    void removeSystem(Ref<System> system);

    void tick(float dt);

    void render();

   private:
    std::set<Ref<System>> m_systems;
};

}  // namespace sd

#endif /* SD_SYSTEM_HPP */
