#ifndef SD_EVENT_DISPATCHER_HPP
#define SD_EVENT_DISPATCHER_HPP

#include "Utility/Base.hpp"

#include <cstdint>
#include <functional>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <any>

namespace SD {

using Callback = std::function<void(std::any)>;

class EventDispatcher;

class HandlerRegistration {
    const void* m_handle{nullptr};
    EventDispatcher* m_dispatcher{nullptr};

   public:
    HandlerRegistration(const HandlerRegistration& other) = delete;
    HandlerRegistration(HandlerRegistration&& other) noexcept
        : m_handle(std::exchange(other.m_handle, nullptr)),
          m_dispatcher(std::exchange(other.m_dispatcher, nullptr)) {}
    HandlerRegistration& operator=(const HandlerRegistration& other) = delete;
    HandlerRegistration& operator=(HandlerRegistration&& other) {
        m_handle = std::exchange(other.m_handle, nullptr);
        m_dispatcher = std::exchange(other.m_dispatcher, nullptr);
        return *this;
    };
    ~HandlerRegistration() { Unregister(); }

    const void* GetHandle() const { return m_handle; }
    void Unregister() noexcept;
    void Reset();

    HandlerRegistration() = default;
    HandlerRegistration(const void* handle, EventDispatcher* bus)
        : m_handle(handle), m_dispatcher(bus) {}
};

// WARNING:NOT Thread-safe
class EventDispatcher {
   public:
    template <typename F, typename EVENT>
    HandlerRegistration Register(F* object, void (F::*method)(const EVENT&)) {
        const auto type_idx = std::type_index(typeid(EVENT));
        auto iter = m_callbacks.emplace(type_idx, [object, method](auto value) {
            (object->*method)(std::any_cast<EVENT>(value));
        });
        const void* handle = static_cast<const void*>(&(iter->second));
        return {handle, this};
    }

    bool RemoveHandler(HandlerRegistration& registration) {
        if (!registration.GetHandle()) return false;

        bool res = false;
        for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it) {
            if (static_cast<const void*>(&(it->second)) ==
                registration.GetHandle()) {
                m_callbacks.erase(it);
                res = true;
                registration.Reset();
                break;
            }
        }
        return res;
    }

    template <typename EVENT>
    void PublishEvent(const EVENT& e) {
        const auto type_idx = std::type_index(typeid(EVENT));
        auto [begin, end] = m_callbacks.equal_range(type_idx);
        for (; begin != end; ++begin) {
            begin->second(e);
        }
    }

   private:
    std::unordered_multimap<std::type_index, Callback> m_callbacks;
};

inline void HandlerRegistration::Unregister() noexcept {
    if (m_dispatcher && m_handle) {
        m_dispatcher->RemoveHandler(*this);
    }
}

inline void HandlerRegistration::Reset() {
    m_dispatcher = nullptr;
    m_handle = nullptr;
}

}  // namespace SD

#endif /* SD_EVENT_DISPATCHER_HPP */
