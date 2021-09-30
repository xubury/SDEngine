#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include <set>
#include "Utils/Base.hpp"
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
    friend class Layer;
    Scene *m_scene;
};

}  // namespace sd

#endif /* SD_SYSTEM_HPP */
