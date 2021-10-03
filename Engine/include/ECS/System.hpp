#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include <set>
#include "Utility/Base.hpp"
#include "ECS/Scene.hpp"

namespace sd {

class System {
   public:
    System() = default;
    virtual ~System() = default;

    virtual void onTick(float){};

    virtual void onRender(){};

    virtual void onSceneChange(){};

    virtual void onResize(int, int){};

   protected:
    friend class Layer;
    Scene *m_scene;
};

}  // namespace sd

#endif /* SD_SYSTEM_HPP */
