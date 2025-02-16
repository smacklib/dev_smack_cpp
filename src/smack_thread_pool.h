
#pragma once

// https://www.geeksforgeeks.org/thread-pool-in-cpp/

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

namespace smack::thread_pool
{

    /**
     * A simple thread pool.
     */
    class ThreadPool {

        // Vector to store worker threads
        std::vector<std::thread> threads_;

        // The task queue.
        std::queue<std::function<void()> > tasks_;

        // Synchronize access to shared data.
        std::mutex queue_mutex_;

        // Signal changes in the state of the tasks queue.
        std::condition_variable cv_;

        // Indicate whether the thread pool should stop or not.
        bool stop_ = false;

    public:
        /**
         * Create a thread pool with a given number of threads.
         */
        ThreadPool(size_t num_threads
            = std::thread::hardware_concurrency())
        {
            // Creating worker threads
            for (size_t i = 0; i < num_threads; ++i) {
                threads_.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;

                        // Unlock the queue before
                        // executing the task so that other
                        // threads can perform enqueue tasks
                        {
                            // Locking the queue so that data
                            // can be shared safely
                            std::unique_lock<std::mutex> lock(
                                queue_mutex_);

                            // Waiting until there is a task to
                            // execute or the pool is stopped
                            cv_.wait(lock, [this] {
                                return !tasks_.empty() || stop_;
                                });

                            // exit the thread in case the pool
                            // is stopped and there are no tasks
                            if (stop_ && tasks_.empty()) {
                                return;
                            }

                            // Get the next task from the queue
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }

                        task();
                    }
                    });
            }
        }

        /**
         * Stop the thread pool.
         */
        ~ThreadPool()
        {
            {
                // Lock the queue to update the stop flag safely
                std::unique_lock<std::mutex> lock(queue_mutex_);
                stop_ = true;
            }

            // Notify all threads
            cv_.notify_all();

            // Joining all worker threads to ensure they have
            // completed their tasks
            for (auto& thread : threads_) {
                thread.join();
            }
        }

        // Enqueue task for execution by the thread pool
        void enqueue(std::function<void()> task)
        {
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                tasks_.emplace(move(task));
            }
            cv_.notify_one();
        }

    private:
    };

}
