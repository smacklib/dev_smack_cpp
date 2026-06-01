/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2025 Michael Binz
 */


#include <gtest/gtest.h>

#include <atomic>
#include <future>
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

TEST(Scheduler, scheduleCyclic) {
    std::atomic<int> count{0};
    smack::Scheduler scheduler;

    // scheduleCyclic requires a future startAt; past times (including the epoch
    // default) are rejected and return false.
    auto startAt = std::chrono::system_clock::now() + 50ms;

    ASSERT_TRUE(
        scheduler.scheduleCyclic(
            [&count](){ count++; },
            200ms,
            startAt
        )
    );

    // Allow time for several cycles to execute:
    //   ~50ms  -> 1st run  (count == 1)
    //   ~250ms -> 2nd run  (count == 2)
    //   ~450ms -> 3rd run  (count == 3)
    std::this_thread::sleep_for( 600ms );

    EXPECT_GE(count.load(), 3);
}

TEST(Scheduler, scheduleCyclic_pastStartAt_returnsFalse) {
    smack::Scheduler scheduler;

    // A start time in the past must be rejected.
    auto pastTime = std::chrono::system_clock::now() - 1s;

    EXPECT_FALSE(
        scheduler.scheduleCyclic(
            [](){ /* should never run */ },
            200ms,
            pastTime
        )
    );
}

// Verify that schedule() actually runs the task and it is observable.
TEST(Scheduler, schedule_executes_task) {
    smack::Scheduler scheduler;
    std::promise<void> done;
    auto future = done.get_future();

    ASSERT_TRUE(
        scheduler.schedule(
            [&done](){ done.set_value(); }
        )
    );

    EXPECT_EQ(std::future_status::ready,
              future.wait_for(2s));
}

// Verify that scheduleIn executes the task after the specified delay.
TEST(Scheduler, scheduleIn_executes_after_delay) {
    smack::Scheduler scheduler;
    std::promise<std::chrono::system_clock::time_point> executed_at;
    auto future = executed_at.get_future();

    auto scheduled_at = std::chrono::system_clock::now();
    ASSERT_TRUE(
        scheduler.scheduleIn(
            [&executed_at](){ executed_at.set_value(std::chrono::system_clock::now()); },
            200ms
        )
    );

    ASSERT_EQ(std::future_status::ready, future.wait_for(2s));
    auto elapsed = future.get() - scheduled_at;
    EXPECT_GE(elapsed, 200ms);
}

// Verify that scheduleAt runs a task scheduled at a specific future time.
TEST(Scheduler, scheduleAt_executes_at_future_time) {
    smack::Scheduler scheduler;
    std::promise<void> done;
    auto future = done.get_future();

    auto target = std::chrono::system_clock::now() + 200ms;
    ASSERT_TRUE(
        scheduler.scheduleAt(
            [&done](){ done.set_value(); },
            target
        )
    );

    EXPECT_EQ(std::future_status::ready, future.wait_for(2s));
}

// scheduleAt must reject a time point in the past.
TEST(Scheduler, scheduleAt_past_returns_false) {
    smack::Scheduler scheduler;

    auto past = std::chrono::system_clock::now() - 1s;
    EXPECT_FALSE(
        scheduler.scheduleAt(
            [](){ /* should never run */ },
            past
        )
    );
}

// scheduleCyclic (no startAt) runs the task at least N times.
TEST(Scheduler, scheduleCyclic_no_startAt_runs_multiple_times) {
    std::atomic<int> count{0};
    smack::Scheduler scheduler;

    ASSERT_TRUE(
        scheduler.scheduleCyclic(
            [&count](){ count++; },
            100ms
        )
    );

    // 1st run is immediate; subsequent runs every 100 ms.
    // After ~350 ms we expect >= 3 executions.
    std::this_thread::sleep_for(350ms);

    EXPECT_GE(count.load(), 3);
}

// After stop(), scheduleIn must return false and not schedule any new task.
TEST(Scheduler, scheduleIn_returns_false_after_stop) {
    smack::Scheduler scheduler;
    scheduler.stop();

    EXPECT_FALSE(
        scheduler.scheduleIn(
            [](){ /* must not run */ },
            100ms
        )
    );
}

// After stop(), schedule() must return false.
TEST(Scheduler, schedule_returns_false_after_stop) {
    smack::Scheduler scheduler;
    scheduler.stop();

    EXPECT_FALSE(
        scheduler.schedule(
            [](){ /* must not run */ }
        )
    );
}

// After stop(), scheduleAt() must return false.
TEST(Scheduler, scheduleAt_returns_false_after_stop) {
    smack::Scheduler scheduler;
    scheduler.stop();

    auto future_time = std::chrono::system_clock::now() + 1s;
    EXPECT_FALSE(
        scheduler.scheduleAt(
            [](){ /* must not run */ },
            future_time
        )
    );
}

// Calling stop() a second time must not crash or block.
TEST(Scheduler, stop_is_idempotent) {
    smack::Scheduler scheduler;
    scheduler.stop();
    EXPECT_NO_THROW(scheduler.stop());
}

// get_scheduler() must return the owning scheduler when called from a thunk.
TEST(Scheduler, get_scheduler_returns_self_from_thunk) {
    smack::Scheduler scheduler;
    std::promise<smack::Scheduler*> result;
    auto future = result.get_future();

    ASSERT_TRUE(
        scheduler.schedule(
            [&result](){
                result.set_value(&smack::Scheduler::get_scheduler());
            }
        )
    );

    ASSERT_EQ(std::future_status::ready, future.wait_for(2s));
    EXPECT_EQ(&scheduler, future.get());
}

// get_scheduler() must throw when called outside a thunk context.
TEST(Scheduler, get_scheduler_throws_outside_thunk) {
    EXPECT_THROW(smack::Scheduler::get_scheduler(), std::runtime_error);
}
