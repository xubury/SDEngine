#ifndef SD_LAYER_HPP
#define SD_LAYER_HPP

#include <string>
#include <SDL.h>

#include "Utils/Export.hpp"

namespace sd {

class SD_API Layer {
   public:
    Layer(const std::string &name) : m_name(name), m_blockEvent(false) {}

    virtual ~Layer() = default;

    Layer(const Layer &) = delete;

    Layer &operator=(const Layer &) = delete;

    virtual void onAttach() {}

    virtual void onDetech() {}

    virtual void onTick(float) {}

    virtual void onRender() {}

    virtual void onImGui() {}

    virtual void onEventPoll(const SDL_Event &) {}

    virtual void onEventProcess() {}

    void setBlockEvent(bool block) { m_blockEvent = block; }

    bool isBlockEvent() const { return m_blockEvent; }

   protected:
    std::string m_name;
    bool m_blockEvent;
};

}  // namespace sd

#endif /* SD_LAYER_HPP */
