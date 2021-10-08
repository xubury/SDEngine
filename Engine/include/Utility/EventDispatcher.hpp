#ifndef SD_EVENT_DISPATCHER_HPP
#define SD_EVENT_DISPATCHER_HPP

#include <cstdint>
#include <functional>
#include <vector>
#include <unordered_map>
#include "Utility/Export.hpp"

namespace sd {

template <typename Event>
using EventListener = std::function<void(const Event&)>;

class SD_API EventDispatcher {
   public:
    template <typename F, typename Event>
    void addKeyedListener(F* key, void (F::*listenerMethod)(const Event&)) {
        EventListener<Event> listener = [key, listenerMethod](const Event& e) {
            (key->*listenerMethod)(e);
        };
        auto& listeners = getListeners<Event>();
        listeners.push_back(listener);
        getKeys<Event>()[reinterpret_cast<intptr_t>(key)] =
            listeners.size() - 1;
    }

    template <typename Event, typename F>
    void removeKeyedListener(F* object) {
        auto& keys = getKeys<Event>();
        auto& listeners = getListeners<Event>();

        intptr_t key = reinterpret_cast<intptr_t>(object);
        int index = keys[key];
        keys.erase(key);
        listeners.erase(listeners.begin() + index);

        for (auto& [_, value] : keys)
            if (value > index) value--;
    }

    template <typename Event>
    void dispatchEvent(const Event& e) {
        for (EventListener<Event>& listener : getListeners<Event>())
            listener(e);
    }

   private:
    template <typename Event>
    std::vector<EventListener<Event>>& getListeners() {
        static std::vector<EventListener<Event>> listeners;
        return listeners;
    }

    template <typename Event>
    std::unordered_map<intptr_t, int>& getKeys() {
        static std::unordered_map<intptr_t, int> keys;
        return keys;
    }
};

}  // namespace sd

#endif /* SD_EVENT_DISPATCHER_HPP */
