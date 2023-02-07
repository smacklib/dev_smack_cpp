/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Copyright © 2023 Michael Binz
 */

#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>

namespace smack::util
{

/**
    * An inter-thread queue.
    */
template <typename T>
class Queue
{
private:
    std::condition_variable condition_;

    std::mutex mutex_;

    std::deque<T> queue_;

    bool disposed_;

    auto dispose()
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            disposed_ = true;
        }

        condition_.notify_all();
    }

public:
    Queue() = default;

    ~Queue()
    {
        dispose();
    }

    /**
        * Add an element to the queue.
        */
    void push(T const& value)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push_front(value);
        }
        condition_.notify_one();
    }

    auto popWait(
        bool& wasDisposed) -> std::optional<T>
    {
        std::unique_lock<std::mutex> lock(mutex_);

        condition_.wait(
            lock,
            [=] {
                return disposed_ || not queue_.empty();
            });

        if (disposed_) {
            wasDisposed = true;
            return {};
        }

        wasDisposed = false;

        T result(std::move(queue_.back()));

        queue_.pop_back();

        return std::optional<T>{ result };
    }

    /**
     *
     */
    auto popWait(
        bool& wasDisposed,
        std::chrono::duration<int, std::milli> timeout)
        -> std::optional<T>
    {
        std::unique_lock<std::mutex> lock(mutex_);

        std::cv_status to = condition_.wait_for(
            lock,
            timeout,
            [=] {
                return disposed_ || not queue_.empty();
            });

        if (disposed_) {
            wasDisposed = true;
            return {};
        }

        wasDisposed = false;

        if (to == std::cv_status::timeout) {
            return {};
        }

        T result(std::move(queue_.back()));

        queue_.pop_back();

        return std::optional<T>{ result };
    }

    /**
     * Remove an element from the queue if one exists.
     * If the queue is empty returns an empty optional.
     * Does not block.
     */
    auto popSync() -> std::optional<T> {
        std::unique_lock<std::mutex> lock(mutex_);

        if (queue_.empty()) {
            return {};
        }

        T result(std::move(queue_.back()));

        queue_.pop_back();

        return std::optional<T>{ result };
    }
};

} // namespace smack::util::queue
