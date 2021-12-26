#include "Utility/ThreadPool.hpp"

namespace SD {

ThreadPool::ThreadPool(uint32_t threads) : m_stop(false) {
    for (uint32_t i = 0; i < threads; ++i) {
        m_workers.emplace_back([this] {
            while (true) {
                std::packaged_task<void()> task;
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_condition.wait(
                        lock, [this] { return m_stop || !m_tasks.empty(); });
                    if (m_stop && m_tasks.empty()) {
                        return;
                    }
                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }

                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop = true;
    }
    m_condition.notify_all();
    for (auto &worker : m_workers) {
        worker.join();
    }
}

}  // namespace SD
