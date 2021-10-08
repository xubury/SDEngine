#ifndef SD_SYSTEM_HPP
#define SD_SYSTEM_HPP

#include <set>
#include "Utility/EventDispatcher.hpp"
#include "Utility/Base.hpp"
#include "ECS/Scene.hpp"
#include "Core/Layer.hpp"

namespace sd {

struct SizeEvent {
    SizeEvent(int width, int height) : width(width), height(height) {}
    int width;
    int height;
};

struct SceneEvent {
    SceneEvent(Scene *scene) : scene(scene) {}
    Scene *scene;
};

class System {
   public:
    System() = default;
    virtual ~System() = default;

    virtual void onInit(){};

    virtual void onDestroy(){};

    virtual void onTick(float){};

    virtual void onRender(){};

    template <typename F, typename Event>
    void registerEvent(F *key, void (F::*listenerMethod)(const Event &)) {
        m_layer->getDispatcher().addKeyedListener(key, listenerMethod);
    }

    template <typename Event, typename F>
    void unregisterEvent(F *object) {
        m_layer->getDispatcher().removeKeyedListener<Event>(object);
    }

   private:
    friend class Layer;
    Layer *m_layer;
};

}  // namespace sd

#endif /* SD_SYSTEM_HPP */
