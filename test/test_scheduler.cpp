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

#include <smack_scheduler.h>
#include <smack_threadpool.h>

auto consumer( smack::THUNK t ) -> void
{
    std::cerr << __LINE__ << " Consumed.\n";
    t();
}

TEST(Scheduler, plain) {
    smack::Scheduler scheduler;

    scheduler.scheduleIn(
        [](){ std::cerr << __LINE__ << " Executed 2.\n"; },
        2s );
    scheduler.scheduleIn(
        [](){ std::cerr << __LINE__ << " Executed.1\n"; },
        1s );

    std::this_thread::sleep_for( 5s );
}

TEST(Scheduler, consumer) {
    smack::Scheduler scheduler(consumer);

    scheduler.scheduleIn(
        [](){ std::cerr << __LINE__ << " Executed 2.\n"; },
        2s );
    scheduler.scheduleIn(
        [](){ std::cerr << __LINE__ << " Executed.1\n"; },
        1s );

    std::this_thread::sleep_for( 5s );
}

void cyclicTask( smack::Scheduler& scheduler, int count )
{
    std::cerr << __LINE__ << " Cyclic run " << count << ".\n";

    if (count > 0) {
        scheduler.scheduleIn(
            [&, count](){ cyclicTask(scheduler, count - 1); },
            250ms );
    }
}

TEST(Scheduler, cyclic) {
    smack::Scheduler scheduler(consumer);

    ASSERT_TRUE(
        scheduler.schedule(
            [&scheduler](){ cyclicTask(scheduler, 4); }
        )
    );

    std::this_thread::sleep_for( 5s );
}

TEST(Scheduler, cyclicWithPool) {
    smack::ThreadPool pool;
    smack::Scheduler scheduler( [&pool]( smack::THUNK t ){
        pool.exec( move(t) );
    } );

    ASSERT_TRUE(
        scheduler.schedule(
            [&scheduler](){ cyclicTask(scheduler, 4); }
        )
    );

    std::this_thread::sleep_for( 5s );
}

TEST(Scheduler, cyclicWithPool_quickExit) {
    smack::ThreadPool pool;
    smack::Scheduler scheduler( [&pool]( smack::THUNK t ){
        pool.exec( move(t) );
    } );

    ASSERT_TRUE(
        scheduler.schedule(
            [&scheduler](){ cyclicTask(scheduler, 4); }
        )
    );
}
