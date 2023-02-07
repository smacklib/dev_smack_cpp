/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Copyright © 2023 Michael Binz
 */

#include <gtest/gtest.h>

#include <string>

#include "smack_queue.hpp"

using std::string;

#include "gtest/gtest.h"

#include <array>
#include <iostream>

using std::string;
using std::vector;

TEST(UtilQueue, Simple) {
    smack::util::Queue<int> q;
    q.push( 313 );
    q.push( 314 );

    {
        auto a = q.popSync();
        ASSERT_TRUE( a );
        ASSERT_EQ( 313, a.value() );
    }
    {
        auto a = q.popSync();
        ASSERT_TRUE( a );
        ASSERT_EQ( 314, a.value() );
    }
    {
        auto a = q.popSync();
        ASSERT_FALSE( a );
    }
}
