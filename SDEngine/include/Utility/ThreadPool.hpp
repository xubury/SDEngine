#ifndef SD_THREAD_POOL_HPP
#define SD_THREAD_POOL_HPP

#include "Utility/Base.hpp"
#include "Utility/Export.hpp"

#include <functional>
#include <future>
#include <queue>

namespace SD {

class SD_UTILITY_API ThreadPool {
   public:
    ThreadPool(uint32_t threads);
    ~ThreadPool();

    template <typename F, typename... ARGS>
    decltype(auto) Queue(F&& f, ARGS&&... args);

   private:
    std::vector<std::thread> m_workers;
    std::queue<std::packaged_task<void()>> m_tasks;

    std::mutex m_mutex;
    bool m_stop;
    std::condition_variable m_condition;
};

template <class F, class... ARGS>
decltype(auto) ThreadPool::Queue(F&& f, ARGS&&... args) {
    using return_type = typename std::result_of<F(ARGS...)>::type;

    std::packaged_task<return_type()> task(
        std::bind(std::forward<F>(f), std::forward<ARGS>(args)...));

    std::future<return_type> res = task.get_future();
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // don't allow enqueueing after stopping the pool
        if (m_stop) throw std::runtime_error("enqueue on stopped ThreadPool");

        m_tasks.emplace(std::move(task));
    }
    m_condition.notify_one();
    return res;
}

}  // namespace SD

#endif  // !SD_THREAD_POOL_HPP
