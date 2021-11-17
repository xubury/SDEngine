#ifndef SD_ACTION_MAP_HPP
#define SD_ACTION_MAP_HPP

#include <unordered_map>
#include "Input/Action.hpp"

namespace SD {

template <typename T = int>
class ActionMap {
   public:
    ActionMap() = default;

    ActionMap(const ActionMap<T> &) = delete;

    ActionMap<T> &operator=(const ActionMap<T> &) = delete;

    void Map(const T &key, const Action &action);

    const Action &Get(const T &key) const;

   private:
    std::unordered_map<T, Action> m_map;
};

template <typename T>
void ActionMap<T>::Map(const T &key, const Action &action) {
    m_map.emplace(key, action);
}

template <typename T>
const Action &ActionMap<T>::Get(const T &key) const {
    return m_map.at(key);
}

}  // namespace SD

#endif /* SD_ACTION_MAP_HPP */
