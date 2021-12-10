#include "Utility/ThreadPool.hpp"

namespace SD {

ThreadPool::ThreadPool(size_t threads) : m_workers(threads), m_stop(false) {}

void ThreadPool::Run() {
    for (auto &worker : m_workers) {
        worker = std::thread([this]() {
            while (true) {
                std::packaged_task<void()> task;
                {
                    std::unique_lock<std::mutex> lock(m_mutex);
                    m_condition.wait(
                        lock, [this] { return m_stop || m_tasks.empty(); });
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

void ThreadPool::Shutdown() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop = true;
    }
    m_condition.notify_all();
    for (auto &worker : m_workers) {
        worker.join();
    }
}

ThreadPool::~ThreadPool() {
    if (!m_stop) Shutdown();
}

}  // namespace SD
