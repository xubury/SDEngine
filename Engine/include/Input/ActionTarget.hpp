#ifndef SD_ACTION_TARGET_HPP
#define SD_ACTION_TARGET_HPP

#include "Input/ActionMap.hpp"

#include <list>
#include <functional>
#include <utility>

namespace SD {

template <typename T>
class ActionTarget {
   public:
    using FuncType = std::function<void(const Event &)>;

    using KeyPair = std::pair<T, FuncType>;

    using ActionPair = std::pair<Action, FuncType>;

   public:
    ActionTarget(const ActionTarget<T> &) = delete;

    ActionTarget<T> &operator=(const ActionTarget<T> &) = delete;

    ActionTarget(const ActionMap<T> &map);

    bool ProcessEvent(const Event &event) const;

    void ProcessEvents() const;

    void Bind(const Action &acton, const FuncType &callback);

    void Bind(Action &&action, const FuncType &callback);

    void Bind(const T &key, const FuncType &callback);

    void Unbind(const T &key);

   private:
    std::list<KeyPair> m_eventRealTime;
    std::list<KeyPair> m_eventPoll;
    std::list<ActionPair> m_eventRealTimeAction;
    std::list<ActionPair> m_eventPollAction;

    const ActionMap<T> &m_actionMap;
};

template <typename T>
ActionTarget<T>::ActionTarget(const ActionMap<T> &map) : m_actionMap(map) {}

template <typename T>
bool ActionTarget<T>::ProcessEvent(const Event &event) const {
    for (const auto &[action, func] : m_eventPollAction) {
        if (action == event) {
            func(event);
            return true;
        }
    }
    for (const auto &[key, func] : m_eventPoll) {
        if (m_actionMap.Get(key) == event) {
            func(event);
            return true;
        }
    }
    return false;
}

template <typename T>
void ActionTarget<T>::ProcessEvents() const {
    for (const auto &[action, func] : m_eventRealTimeAction) {
        if (action.Test()) {
            func(action.m_event);
        }
    }
    for (const auto &[key, func] : m_eventRealTime) {
        const Action &action = m_actionMap.Get(key);
        if (action.Test()) {
            func(action.m_event);
        }
    }
}

template <typename T>
void ActionTarget<T>::Bind(const T &key, const FuncType &callback) {
    const Action &action = m_actionMap.Get(key);
    if (action.m_type & Action::Type::REAL_TIME) {
        m_eventRealTime.emplace_back(key, callback);
    } else {
        m_eventPoll.emplace_back(key, callback);
    }
}

template <typename T>
void ActionTarget<T>::Bind(const Action &action, const FuncType &callback) {
    if (action.m_type & Action::Type::REAL_TIME) {
        m_eventRealTimeAction.emplace_back(action, callback);
    } else {
        m_eventPollAction.emplace_back(action, callback);
    }
}

template <typename T>
void ActionTarget<T>::Bind(Action &&action, const FuncType &callback) {
    if (action.m_type & Action::Type::REAL_TIME) {
        m_eventRealTimeAction.emplace_back(std::move(action), callback);
    } else {
        m_eventPollAction.emplace_back(std::move(action), callback);
    }
}

template <typename T>
void ActionTarget<T>::Unbind(const T &key) {
    const Action &action = m_actionMap.Get(key);
    auto removeFunc = [&key](const ActionPair &pair) -> bool {
        return pair.first == key;
    };
    if (action.m_type & Action::Type::REAL_TIME) {
        m_eventRealTime.remove_if(removeFunc);
    } else {
        m_eventPoll.remove_if(removeFunc);
    }
}

}  // namespace SD

#endif /* SD_ACTION_TARGET_HPP */
