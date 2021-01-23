#include <gtest/gtest.h> // googletest header file

#include <string>

#include "src/util.hpp"

using std::string;

namespace {
    using std::cout;
    using std::endl;
    using smack::util::Disposer;

    int closedCount = 0;

    int argOpen()
    {
        auto seven = 7;
        cout << "Opening " << seven << endl;
        return seven;
    }

    int argDo(int what)
    {
        cout << "Doing " << what << endl;
        return 313;
    }
    void argClose(int what)
    {
        closedCount++;
        cout << "Closed " << what << endl;
    }

    template <typename H, typename F>
    auto make(
        H handle,
        F function)
    {
        Disposer<decltype(handle), decltype(function)>
            result(handle, function);
        return result;
    }
}

TEST(SmackUtilTest, Disposer) {
    auto handle = make(
        argOpen(),
        argClose);

    // The disposer must be moved, the close operation must not be called.
    EXPECT_EQ( 0, closedCount );

    argDo(handle);

    std::cout << "Hello World!\n";
}
