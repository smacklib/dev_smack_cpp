/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2025 Michael Binz
 */

#include <gtest/gtest.h>

#include <smack_threadpool.h>
#include <smack_util_time_probe.hpp>

struct ConcurrencyStats : std::mutex
{
    size_t maxConcurrency_{ 0 };
    size_t crtConcurrency_{ 0 };

    void inc()
    {
        std::unique_lock<std::mutex> lock(*this);

        crtConcurrency_++;
        if (crtConcurrency_ > maxConcurrency_) {
            maxConcurrency_ = crtConcurrency_;
        }
    }
    void dec()
    {
        std::unique_lock<std::mutex> lock(*this);

        crtConcurrency_--;
    }
};

void testThunk(size_t taskNumber, ConcurrencyStats& stats)
{
    stats.inc();

    std::cerr << "Working\n";
        //"Task %d: tid=%d",
        //taskNumber,
    //);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    stats.dec();
}

TEST(ThreadPool, plain) {
    smack::ThreadPool pool{ 3 };
    ConcurrencyStats stats;

    for (int i = 0; i < 3 * pool.size(); ++i) {
        pool.exec([i, &stats] {testThunk(i, stats); });
    }

    smack::util::TimeProbe tp{ __FUNCTION__ };
    pool.stop();

    auto duration = tp.duration();

    ASSERT_GT(duration, .5);

    ASSERT_EQ(pool.size(), stats.maxConcurrency_);
}

auto thunk( size_t count ) -> void
{
    std::cerr << "thunk " << count << "\n";

    if ( count == 0 ) {
        std::cerr << "done\n";
        return;
    }
    count--;

    smack::ThreadPool::get_pool().exec(
        [count]()
        {
            thunk( count );
        }
    );
};

TEST(ThreadPool, chained) {
    smack::ThreadPool pool{ 3 };

    pool.exec(
        [](){
            thunk( 20 );
        }
    );

    std::this_thread::sleep_for( std::chrono::milliseconds(100) );
}

TEST(ThreadPool, getPool_noThunk) {
    ASSERT_THROW(
        smack::ThreadPool::get_pool(),
        std::runtime_error
     );
}

TEST(TimeProbe, research)
{
    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;
    auto t0 = Time::now();
    auto t1 = Time::now();
    fsec fs = t1 - t0;
    ms d = std::chrono::duration_cast<ms>(fs);
    std::cout << fs.count() << "s\n";
    std::cout << d.count() << "ms\n";
}

// TEST( TimeProbe, plain )
// {
//     // std::unique_ptr<size_t> kolme = std::make_unique<size_t>(313);
//     // std::function<void()> f1 = [this, methodResult = std::move(kolme)]() {};
//     uint64_t sec = 1000;

//     cmi::TimeProbe2 tp{  __FUNCTION__ };
//     cmi::TimeProbe::sleepMs( sec );
//     auto durationMs = tp.durationMs();

//     ASSERT_LT( 999, durationMs );
//     ASSERT_LT( durationMs, 1010 );
// }

// class SomeType {
// public:
//     SomeType(int value) : data(value) {}
//     int data;
// };

// int test() {
//     std::unique_ptr<SomeType> result = std::make_unique<SomeType>(42);

//     std::function<void()> task = [result = std::move(result)]() {
//         std::cout << "Value: " << result->data << std::endl;
//     };

//     task(); // Execute the task

//     return 0;
// }
