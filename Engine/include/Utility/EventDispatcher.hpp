#ifndef SD_EVENT_DISPATCHER_HPP
#define SD_EVENT_DISPATCHER_HPP

#include "Utility/Base.hpp"

#include <cstdint>
#include <functional>
#include <vector>
#include <unordered_map>

namespace SD {

// WARNING:NOT Thread-safe
template <typename EVENT>
using Callback = std::function<void(const EVENT&)>;

class SD_API EventDispatcher {
   public:
    template <typename F, typename EVENT>
    void subscribe(F* object, void (F::*METHOD)(const EVENT&)) {
        Callback<EVENT> callback = [object, METHOD](const EVENT& e) {
            (object->*METHOD)(e);
        };
        auto& callbacks = getCallbacks<EVENT>();
        callbacks.push_back(callback);
        intptr_t key = reinterpret_cast<intptr_t>(object);
        auto& keys = getKeys<EVENT>();
        if (keys.count(key) == 0) {
            keys[key] = callbacks.size() - 1;
        } else {
            SD_CORE_ERROR("EventDispatcher::subscribe: Repeated subscrition!");
        }
    }

    template <typename EVENT, typename F>
    void unsubscribe(F* object) {
        auto& keys = getKeys<EVENT>();
        auto& listeners = getCallbacks<EVENT>();

        intptr_t key = reinterpret_cast<intptr_t>(object);
        int index = keys[key];
        keys.erase(key);
        listeners.erase(listeners.begin() + index);

        for (auto& [_, value] : keys)
            if (value > index) value--;
    }

    template <typename EVENT>
    void publishEvent(const EVENT& e) {
        for (Callback<EVENT>& listener : getCallbacks<EVENT>()) listener(e);
    }

   private:
    template <typename EVENT>
    std::vector<Callback<EVENT>>& getCallbacks() {
        static std::vector<Callback<EVENT>> callbacks;
        return callbacks;
    }

    template <typename EVENT>
    std::unordered_map<intptr_t, int>& getKeys() {
        static std::unordered_map<intptr_t, int> keys;
        return keys;
    }
};

}  // namespace SD

#endif /* SD_EVENT_DISPATCHER_HPP */
