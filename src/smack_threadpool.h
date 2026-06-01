/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * A task scheduler.
 *
 * Copyright © 2025-2026 Michael Binz
 */

#pragma once

// https://www.geeksforgeeks.org/thread-pool-in-cpp/

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

#include "smack_common.h"

namespace smack {

/**
 * A thread pool.
 */
class ThreadPool {
public:

private:
    // The worker threads.
    std::vector<std::thread> threads_;

    // The task queue.
    std::queue<THUNK> tasks_;

    // Signals changes in the tasks queue.
    std::condition_variable cv_;

    // Protects tasks_ and stop_.
    std::mutex mutex_;

    // If true the thread pool is in the shutdown process.
    bool stop_ = false;

    // A transaction counter.
    std::atomic<size_t> tidCount_{0};

    // The current thread's transaction id.  Zero means "not a pool thread";
    // valid IDs start at 1 because tidCount_ is pre-incremented before use.
    inline static thread_local size_t transactionId_;

    // A reference to the current thread pool.  nullptr means "not a pool thread".
    inline static thread_local ThreadPool* self_;

public:
    /**
     * Create an instance.
     */
    static constexpr size_t DEFAULT_THREAD_COUNT = 5;

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    ThreadPool(
        size_t threadCount = std::thread::hardware_concurrency() != 0
            ? std::thread::hardware_concurrency()
            : DEFAULT_THREAD_COUNT)
    {
        if (threadCount == 0) {
            throw std::invalid_argument("threadCount must be greater than zero.");
        }

        for (size_t i = 0; i < threadCount; ++i) {

            threads_.emplace_back([this] {
                self_ = this;

                while (true) {
                    THUNK task;

                    {
                        std::unique_lock<std::mutex> lock(mutex_);

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

    /**
     * Get the thread pool that manages the current thread.  This can be
     * used to chain executions, that is, to register a new task from a
     * running task.  Note that this must be called from a thread
     * managed by the pool.
     *
     * @throws std::runtime_error If the calling thread is not a pool thread.
     */
    static auto get_pool() -> ThreadPool&
    {
        if (self_ == nullptr) {
            throw std::runtime_error("Not called from thunk.");
        }

        return *self_;
    }

    /**
     * Get the calling thread's transaction id.  Note that this must
     * be called from a thread managed by the pool.
     *
     * @throws std::runtime_error If the calling thread is not a pool thread.
     */
    static auto get_transaction_id() -> size_t
    {
        if (transactionId_ == 0) {
            throw std::runtime_error("Not called from thunk.");
        }

        return transactionId_;
    }

    /**
     * Stop the thread pool.  Before the pool is stopped, threads in the
     * pool's queue are processed until the queue is empty.
     *
     * @throws std::runtime_error If called from a thread managed by this pool.
     */
    void stop()
    {
        if (self_ == this) {
            throw std::runtime_error("stop() must not be called from a pool thread.");
        }

        {
            std::unique_lock<std::mutex> lock(mutex_);

            // Ignore if already stopped.
            if (stop_) {
                return;
            }

            stop_ = true;
        }

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
     *
     * @param task The task to execute.
     * @throws std::runtime_error if the threadpool is already stopped.
     */
    void exec(THUNK task)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);

            if (stop_) {
                throw std::runtime_error("pool already stopped.");
            }

            tasks_.emplace(move(task));
        }

        cv_.notify_one();
    }

    /**
     * Get the size of the thread pool as passed in the constructor.
     */
    auto size() const -> size_t
    {
        return threads_.size();
    }

    /**
     * Get the number of transactions the threadpool has executed.
     */
    auto transaction_count() const -> size_t
    {
        return tidCount_;
    }
};

} // namespace smack
