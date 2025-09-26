/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2025 Michael Binz
 */


#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <thread>

#include <smack_thread_pool.h>

using std::string;


TEST(ThreadPool, plain) {
    // Create a thread pool with 4 threads
    smack::thread_pool::ThreadPool pool(4);

    // Enqueue tasks for execution
    for (int i = 0; i < 5; ++i) {
        pool.enqueue([i] {
            GTEST_LOG_(INFO) << "Task " << i << " is running on thread "
                << std::this_thread::get_id() << std::endl;
            // Simulate some work
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100));
            });
    }

}

