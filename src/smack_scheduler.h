/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * A task scheduler.
 *
 * Copyright © 2025 Michael Binz
 */

#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <stdexcept>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

#include "smack_common.h"

// A hash function for std::chrono::time_point is missing in C++17.
// C++26 delivers it.  This is a workaround.
namespace std {
template<typename _rep, typename ratio>
struct hash<std::chrono::time_point<_rep, ratio>> {
    typedef std::chrono::time_point<_rep, ratio> argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const
    {
        return std::hash<_rep>{}(s.count());
    }
};
} // namespace std

namespace smack {

/**
 * A task scheduler.
 */
class Scheduler : private std::mutex {

    static auto internalConsumer( THUNK thunk ) -> void
    {
        std::thread( [thunk = std::move(thunk)]() {
            thunk();
        }).detach();
    }

    CONSUMER consumer_;

    /**
     * A thread that performs the scheduling.
     */
    std::thread dispatcher_;

    // The scheduled tasks in sorted order.
    std::multimap<std::chrono::time_point<std::chrono::system_clock>, THUNK> ptasks_;

    // Signals changes in the tasks queue.
    std::condition_variable cv_;

    // If true the scheduler is in the shutdown process.
    std::atomic<bool> stop_ = false;

    // A reference to the current Scheduler.
    inline static thread_local Scheduler* self_;

    auto dispatch() -> void
    {
        self_ = this;

        while (!stop_) {
            auto now = std::chrono::system_clock::now();

            THUNK to_execute;

            {
                std::unique_lock<std::mutex> lock(*this);

                auto first = ptasks_.begin();
                if (first == ptasks_.end()) {
                    // No tasks, wait until a new one is scheduled.
                    cv_.wait(lock);

                    continue;
                }

                auto [next, task] = *first;
                if (next > now) {
                    // Wait until the next task is due or a new task is scheduled.
                    cv_.wait_until(lock, next);

                    continue;
                }

                to_execute = std::move(task);

                ptasks_.erase(first);
            }

            consumer_(to_execute);
        }
    }

public:
    /**
     * Create an instance that allows to pass an external consumer.
     *
     * @param consumer The consumer to execute the scheduled tasks.
     */
    Scheduler(CONSUMER consumer)
        : consumer_{consumer}
        , dispatcher_{[this]() { dispatch(); }}
    {
    }

    /**
     * Create an instance using an internal consumer.
     *
     * @param consumer The consumer to execute the scheduled tasks.
     */
    Scheduler()
        : consumer_{internalConsumer}
        , dispatcher_{[this]() { dispatch(); }}
    {
    }

    /**
     * Stop the scheduler.  Note that this blocks until all threads
     * in the pool finished.
     */
    ~Scheduler()
    {
        stop();
    }

    static auto get_scheduler() -> Scheduler&
    {
        if (self_ == nullptr) {
            throw std::runtime_error("Not called from thunk.");
        }

        return *self_;
    }

    /**
     * Stop scheduling.
     */
    void stop()
    {
        {
            // Modify the stop flag under lock.
            std::unique_lock<std::mutex> lock(*this);

            // Ignore if already stopped.
            if (stop_) {
                return;
            }

            stop_ = true;
        }

        cv_.notify_one();

        dispatcher_.join();
    }

    /**
     * Register a task for scheduling.
     *
     * @return false if the scheduler is already stopped, otherwise true.
     */
    auto scheduleIn(THUNK task, Duration duration) -> bool
    {
        if (stop_) {
            return false;
        }

        {
            std::unique_lock<std::mutex> lock(*this);
            ptasks_.emplace(
                std::chrono::system_clock::now() + duration,
                // Decided for move, could copy?
                move(task));
        }

        cv_.notify_one();

        return true;
    }

    /**
     * Schedule a task now.
     *
     * @return false if the scheduler is already stopped, otherwise true.
     */
    auto schedule(THUNK task) -> bool
    {
        return scheduleIn(
            move(task),
            0s );
    }

    /**
     * Register a task for scheduling.
     *
     * @return false if the scheduler is already stopped, otherwise true.
     */
    auto scheduleAt(THUNK task, TimePoint time) -> bool
    {
        if (stop_) {
            return false;
        }

        if ( time < std::chrono::system_clock::now() ) {
            return false;
        }

        {
            std::unique_lock<std::mutex> lock(*this);
            ptasks_.emplace(
                time,
                // Decided for move, could copy?
                move(task));
        }

        cv_.notify_one();

        return true;
    }
};

} // namespace smack
