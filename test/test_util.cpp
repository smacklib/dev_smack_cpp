#include <gtest/gtest.h> // googletest header file

#include <string>

#include "src/smack_util.hpp"

using std::string;

namespace {
    using std::cout;
    using std::endl;
    using smack::util::Disposer;

    int closedCount = 0;

    int argOpen() {
        auto seven = 7;
        return seven;
    }

    int argDo(int what) {
        return 313;
    }

    void argClose(int what) {
        closedCount++;
    }
}

TEST(SmackUtilTest, Disposer) {
    {
        Disposer handle{ argOpen(), argClose };
        EXPECT_EQ(7, handle);
        EXPECT_EQ(0, closedCount);
        EXPECT_EQ(313, argDo(handle));
    }
    EXPECT_EQ(1, closedCount);
}
