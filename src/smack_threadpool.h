/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * A simpe threadpool implementation.
 *
 * Copyright © 2025 Michael Binz
 */

#pragma once

// https://www.geeksforgeeks.org/thread-pool-in-cpp/

#include <array>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace smack {

/**
 * A thread pool.
 */
class ThreadPool : private std::mutex {
public:
    using C = std::function<void()>;

private:
    // The worker threads.
    std::vector<std::thread> threads_;

    // The task queue.
    std::queue<C> tasks_;

    // Signals changes in the tasks queue.
    std::condition_variable cv_;

    // If true the thread pool is in the shutdown process.
    std::atomic<bool> stop_ = false;

    // A transaction counter.
    std::atomic<size_t> tidCount_;

    // The current thread's transaction id.
    inline static thread_local size_t transactionId_;

    // A reference to the current thread pool.
    inline static thread_local ThreadPool* self_;

public:
    /**
     * Create an instance.
     */
    ThreadPool(size_t threadCount = std::thread::hardware_concurrency())
    {
        for (size_t i = 0; i < threadCount; ++i) {

            threads_.emplace_back([this] {
                self_ = this;

                while (true) {
                    C task;

                    {
                        std::unique_lock<std::mutex> lock(*this);

                        // Wait until there is a task to execute or the pool is
                        // stopped.
                        cv_.wait(lock, [this] { return !tasks_.empty() || stop_; });

                        // Terminate the thread.
                        if (stop_ && tasks_.empty()) {
                            return;
                        }

                        // Get the next task from the queue.
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }

                    transactionId_ = ++tidCount_;
                    task();
                }
            });
        }
    }

    /**
     * Stop the thread pool.  Note that this blocks until all threads
     * in the pool finished.
     */
    ~ThreadPool()
    {
        stop();
    }

    static auto get_pool() -> ThreadPool&
    {
        if (self_ == nullptr) {
            throw std::runtime_error("Not called from thunk.");
        }

        return *self_;
    }

    /**
     * Stop the thread pool.  Before the pool is stopped, threads in the
     * pool's queue are processed until the queue is empty.
     */
    void stop()
    {
        // Ignore if already stopped.
        if (stop_) {
            return;
        }

        stop_ = true;

        // Notify all threads
        cv_.notify_all();

        // Joining the worker threads.  This blocks until the last worker
        // finishes.
        for (auto& thread : threads_) {
            thread.join();
        }
    }

    /**
     * Register a task for execution by the thread pool.
     */
    void exec(C task)
    {
        if (stop_) {
            throw std::runtime_error("pool already stopped.");
        }

        {
            std::unique_lock<std::mutex> lock(*this);
            tasks_.emplace(move(task));
        }

        cv_.notify_one();
    }

    auto size() -> size_t
    {
        return threads_.size();
    }

    auto transaction_count() -> size_t
    {
        return tidCount_;
    }
};

} // namespace smack
