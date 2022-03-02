#ifndef SD_EVENT_STACK_HPP
#define SD_EVENT_STACK_HPP

#include "Utility/Base.hpp"
#include <vector>

namespace SD {

template <typename T>
class SD_UTILITY_API EventStack {
    using iterator = typename std::vector<T>::iterator;
    using reverse_iterator = typename std::vector<T>::reverse_iterator;

   public:
    EventStack() : m_insertId(0) {}

    ~EventStack() = default;

    void Push(T item) {
        m_items.emplace(m_items.begin() + m_insertId, item);
        ++m_insertId;
    }

    void PushOverlay(T item) { m_items.emplace_back(item); }

    void Pop(T item) {
        auto iter = m_items.begin();
        for (; iter != m_items.end(); iter++) {
            if (*iter == item) {
                break;
            }
        }
        if (iter != m_items.end()) {
            size_t id = iter - m_items.begin();
            m_items.erase(iter);
            if (id < m_insertId) {
                --m_insertId;
            }
        } else {
            SD_CORE_ERROR("EventStack::PopLayer Failed! No layer found!");
        }
    }

    bool Has(T item) const {
        return std::find(m_items.begin(), m_items.end(), item) != m_items.end();
    }

    iterator begin() { return m_items.begin(); }

    iterator end() { return m_items.end(); }

    reverse_iterator rbegin() { return m_items.rbegin(); }

    reverse_iterator rend() { return m_items.rend(); }

    const iterator begin() const { return m_items.begin(); }

    const iterator end() const { return m_items.end(); }

    const reverse_iterator rbegin() const { return m_items.rbegin(); }

    const reverse_iterator rend() const { return m_items.rend(); }

    size_t Size() const { return m_items.size(); }

    T &Front() { return m_items.front(); }
    const T &Front() const { return m_items.front(); }

   private:
    std::vector<T> m_items;
    size_t m_insertId;
};

}  // namespace SD

#endif /* SD_EVENT_STACK_HPP */
