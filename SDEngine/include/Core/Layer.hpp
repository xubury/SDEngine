#ifndef SD_LAYER_HPP
#define SD_LAYER_HPP

#include "Utility/Base.hpp"
#include "Core/Event.hpp"
#include "Core/EventStack.hpp"
#include "Core/Export.hpp"

namespace SD {

class SD_CORE_API Layer {
   public:
    Layer(const std::string &name) : m_name(name)
    {
        SD_CORE_TRACE("Initializing layer: {}", name);
    }

    virtual ~Layer() { SD_CORE_TRACE("Deleteing layer: {}", m_name); }

    Layer(const Layer &) = delete;

    Layer &operator=(const Layer &) = delete;

    virtual void OnInit() {}

    virtual void OnDestroy() {}

    virtual void OnPush() {}

    virtual void OnPop() {}

    virtual void OnTick(float) {}

    virtual void On(const WindowSizeEvent &) {}
    virtual void On(const KeyEvent &) {}
    virtual void On(const MouseMotionEvent &) {}
    virtual void On(const MouseButtonEvent &) {}
    virtual void On(const MouseWheelEvent &) {}
    virtual void On(const TextInputEvent &) {}
    virtual void On(const AppQuitEvent &) {}

    virtual void OnRender() {}

    virtual void OnImGui() {}

    const std::string &GetName() const { return m_name; }

   private:
    friend class Application;

    std::string m_name;
};

}  // namespace SD

#endif /* SD_LAYER_HPP */
