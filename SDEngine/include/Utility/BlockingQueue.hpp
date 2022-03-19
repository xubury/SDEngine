#ifndef SD_BLOCKING_QUEUE_HPP
#define SD_BLOCKING_QUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class BlockingQueue {
   public:
    using container = typename std::queue<T>;
    using iterator = typename container::iterator;
    using const_iterator = typename container::const_iterator;

   private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    container m_queue;

   public:
    /**
     * Pushes an item to the back of the queue.
     *
     * @param item The item to push.
     */
    void Push(const T& item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(item);
        m_condition.notify_one();
    }

    /**
     * Pops an item from the front of the queue.
     */
    T Pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty()) {
            m_condition.wait(lock, [this] { return !m_queue.empty(); });
        }

        T item = std::move(m_queue.front());
        m_queue.pop();
        return item;
    }

    void Clear()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.clear();

        m_condition.notify_one();
    }

    bool Empty()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    size_t Size()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    iterator begin() { return m_queue.begin(); }

    iterator end() { return m_queue.end(); }

    const_iterator begin() const { return m_queue.begin(); }

    const_iterator end() const { return m_queue.end(); }
};

#endif
