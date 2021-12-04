#ifndef SD_EVENT_DISPATCHER_HPP
#define SD_EVENT_DISPATCHER_HPP

#include "Utility/Base.hpp"

#include <cstdint>
#include <functional>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <shared_mutex>
#include <any>

namespace SD {

using Callback = std::function<void(std::any)>;

class EventDispatcher;

class HandlerRegistration {
   public:
    HandlerRegistration() : m_handle(nullptr), m_dispatcher(nullptr){};
    HandlerRegistration(const void* handle, EventDispatcher* bus)
        : m_handle(handle), m_dispatcher(bus) {}
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

   private:
    const void* m_handle;
    EventDispatcher* m_dispatcher;
};

class EventDispatcher {
   public:
    template <typename EVENT, typename F>
    HandlerRegistration Register(F* object, void (F::*method)(const EVENT&)) {
        const auto type_idx = std::type_index(typeid(EVENT));
        const void* handle;
        SafeUniqueAccess([&]() {
            auto iter =
                m_callbacks.emplace(type_idx, [object, method](auto value) {
                    (object->*method)(std::any_cast<EVENT>(value));
                });
            handle = static_cast<const void*>(&(iter->second));
        });
        return {handle, this};
    }

    // lambda function should give template explicitly, not sure how to make
    // compiler to dedcut it
    template <typename EVENT>
    HandlerRegistration Register(std::function<void(const EVENT&)>&& method) {
        const auto type_idx = std::type_index(typeid(EVENT));
        const void* handle;
        SafeUniqueAccess([&]() {
            auto iter = m_callbacks.emplace(
                type_idx, [func = std::move(method)](auto value) {
                    func(std::any_cast<EVENT>(value));
                });
            handle = static_cast<const void*>(&(iter->second));
        });
        return {handle, this};
    }

    bool RemoveHandler(HandlerRegistration& registration) {
        if (!registration.GetHandle()) return false;
        bool res = false;

        SafeUniqueAccess([&]() {
            for (auto it = m_callbacks.begin(); it != m_callbacks.end(); ++it) {
                if (static_cast<const void*>(&(it->second)) ==
                    registration.GetHandle()) {
                    m_callbacks.erase(it);
                    res = true;
                    registration.Reset();
                    break;
                }
            }
        });
        return res;
    }

    template <typename EVENT>
    void PublishEvent(EVENT&& e) {
        SafeSharedAccess([this, local_evt = std::forward<EVENT>(e)]() {
            auto [begin, end] =
                m_callbacks.equal_range(std::type_index(typeid(EVENT)));
            for (; begin != end; ++begin) {
                begin->second(local_evt);
            }
        });
    }

   private:
    std::unordered_multimap<std::type_index, Callback> m_callbacks;
    using MutexType = std::shared_mutex;
    MutexType m_mutex;

    template <typename CALLBACK>
    void SafeUniqueAccess(CALLBACK&& callback) {
        std::unique_lock<MutexType> lock(m_mutex);
        callback();
    }

    template <typename CALLBACK>
    void SafeSharedAccess(CALLBACK&& callback) {
        std::shared_lock<MutexType> lock(m_mutex);
        callback();
    }
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
