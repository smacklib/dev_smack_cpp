/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * A task scheduler.
 *
 * Copyright © 2025-2026 Michael Binz
 */

#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <stdexcept>
#include <functional>
#include <map>
#include <mutex>
#include <thread>

#include "smack_common.h"

namespace smack {

/**
 * A task scheduler.
 */
class Scheduler {

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

    // Protects ptasks_ and stop_.
    std::mutex mutex_;

    // Signals changes in the tasks queue.
    std::condition_variable cv_;

    // If true the scheduler is in the shutdown process.
    std::atomic<bool> stop_ = false;

    // A reference to the current Scheduler.
    inline static thread_local Scheduler* self_;

    auto dispatch() -> void
    {
        while (!stop_) {
            auto now = std::chrono::system_clock::now();

            THUNK to_execute;

            {
                std::unique_lock<std::mutex> lock(mutex_);

                auto first = ptasks_.begin();
                if (first == ptasks_.end()) {
                    // No tasks, wait until a new one is scheduled.
                    cv_.wait(lock);

                    continue;
                }

                auto& [next, task] = *first;

                if (next > now) {
                    // Wait until the next task is due or a new task is scheduled.
                    cv_.wait_until(lock, next);

                    continue;
                }

                to_execute = std::move(task);

                ptasks_.erase(first);
            }

            consumer_([this, thunk = std::move(to_execute)]() mutable {
                self_ = this;
                // Ensure that self_ is reset to nullptr when the task
                // finishes, even if it throws an exception.
                struct Guard { ~Guard() { self_ = nullptr; } } guard;
                thunk();
            });
        }
    }

    auto cycler(THUNK task, Duration cycleDuration) -> void
    {
        if (stop_) {
            return;
        }

        task();

        scheduleIn(
            [this, task = std::move(task), cycleDuration](){ cycler( std::move(task), cycleDuration ); },
            cycleDuration );
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

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;
    Scheduler(Scheduler&&) = delete;
    Scheduler& operator=(Scheduler&&) = delete;

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
     * Stop scheduling.  All pending tasks are discarded and no new tasks
     * are accepted.
     */
    void stop()
    {
        {
            // Modify the stop flag under lock.
            std::lock_guard<std::mutex>lock{mutex_};

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
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stop_) {
                return false;
            }
            ptasks_.emplace(
                std::chrono::system_clock::now() + duration,
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
        if ( time < std::chrono::system_clock::now() ) {
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (stop_) {
                return false;
            }
            ptasks_.emplace(
                time,
                move(task));
        }

        cv_.notify_one();

        return true;
    }

    /**
     * Schedule a task for cyclic execution.
     *
     * @param task The task to execute. Note that cyclic execution is stopped
     * if the passed task throws an exception.
     * @param cycleDuration The duration between the end of one execution and
     * the start of the next.  Note that this may involve a time drift.
     * @return false if the scheduler is already stopped, otherwise true.
     */
    auto scheduleCyclic(THUNK task, Duration cycleDuration) -> bool
    {
        THUNK cyclerSelf =
            [this, task = std::move(task), cycleDuration]()
            {
                cycler( std::move(task), cycleDuration );
            };

        return schedule( std::move(cyclerSelf) );
    }

    /**
     * Schedule a task for cyclic execution where the cycle starts at a
     * specific time.
     *
     * @param task The task to execute. Note that cyclic execution is stopped
     * if the passed task throws an exception.
     * @param cycleDuration The duration between the end of one execution and
     * the start of the next.  Note that this may involve a time drift.
     * @param startAt The time when the first execution should start.  Note
     * that this must be a future time, otherwise the task is not scheduled
     * and false is returned.
     * @return false if the scheduler is already stopped, otherwise true.
     */
    auto scheduleCyclic(THUNK task, Duration cycleDuration, TimePoint startAt) -> bool
    {
        THUNK cyclerSelf =
            [this, task = std::move(task), cycleDuration]()
            {
                cycler( std::move(task), cycleDuration );
            };

        return scheduleAt( std::move(cyclerSelf), startAt );
    }
};

} // namespace smack
