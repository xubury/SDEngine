#include "Core/Event/Event.hpp"

namespace SD {

static EventDispatcher *s_dispatcher;

void EventSystem::Init() { s_dispatcher = new EventDispatcher; }

void EventSystem::Shutdown()
{
    delete s_dispatcher;
    s_dispatcher = nullptr;
}

EventDispatcher &EventSystem::Get() { return *s_dispatcher; }

};  // namespace SD
