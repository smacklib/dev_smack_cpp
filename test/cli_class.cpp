/*
 * Smack C++
 *
 * Copyright © 2019 Michael Binz
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include "test_common.hpp"

#include "../src/smack_util_console_app.hpp"

using std::cout;
using std::endl;

static int free_function(int p1) {
    std::cout <<
        __func__ <<
        " : " <<
        p1 <<
        std::endl;
    return EXIT_SUCCESS;
}

#if 1
class TestApplication
{
    std::string name_;

public:
    TestApplication(std::string name) :
        name_(name) {
    }

    // Ensures that we do not accidentally copy the host application.
    TestApplication(const TestApplication&) = delete;
    TestApplication& operator=(const TestApplication&) = delete;

    int f1(int p1) {
        return smack::test::common::f(__func__, p1);
    }

    int f2(int p1, const char* p2) {
        return smack::test::common::f(__func__, p1, p2);
    }

    int f3(int p1, double p2, const char* p3) {
        return smack::test::common::f(__func__, p1, p2, p3);
    }

    int f4(const std::string p1) {
        return smack::test::common::f(__func__, p1);
    }

    int f5(const std::string& p1) {
        return smack::test::common::f(__func__, p1);
    }

    int f6(bool p1) {
        return smack::test::common::f(__func__, p1);
    }

    int f7(int p1) const {
        return smack::test::common::f(__func__, p1);
    }
    int f7_2(int p1, double p2) const {
        return smack::test::common::f(__func__, p1, p2);
    }

    int execute(const std::vector<std::string>& argv) {
        using smack::util::Outer;

        auto cmd1 = Outer::makem<&TestApplication::f1>(
            this, 
            "eins");

        auto cmd2 = Outer::makem<&TestApplication::f2>(
            this,
            "zwei");

        auto cmd3 = Outer::makem<&TestApplication::f3>(
            this,
            "drei");

        auto cmd4 = Outer::makem<&TestApplication::f4>(
            this,
            "vier");

        auto cmd5 = Outer::makem<&TestApplication::f5>(
            this,
            "fuenf");

        auto cmd6 = Outer::makem<&TestApplication::f6>(
            this,
            "sechs");

#if 0
        auto cmd7 = Outer::makem<&TestApplication::f7>(
            this,
            "sieben");
#else
        auto cmd7 = smack::util::Commands<
            int> ::make(
                "sieben",
                *this,
                &TestApplication::f7);
#endif

        auto cmd7_2 = smack::util::Commands<
            int,double>::make(
                "sieben_2",
                *this,
                &TestApplication::f7_2);

        auto cli = smack::util::makeCliApplication(
            cmd1,
            cmd2,
            cmd3,
            cmd4,
            cmd5,
            cmd6,
            cmd7,
            cmd7_2,
            Outer::make<free_function>(
                "acht") );

        return cli.launch(argv);
    }
};

int main(int argc, char**argv)
{
    TestApplication ta{ 
        argv[0] 
    };

    std::vector<std::string> cmdArgv(
        argv + 1, 
        argv + argc);

    return ta.execute(cmdArgv);
}

#elif 0

// https://stackoverflow.com/questions/29906242/c-deduce-member-function-parameters
// https://codereview.stackexchange.com/questions/69632/wrap-function-pointers-in-template-classes
// https://developercommunity.visualstudio.com/t/a-template-that-takes-a-member-function-pointer-as/435474

#include <algorithm>
#include <iostream>
#include <functional>

using std::cout;
using std::endl;

template <auto F>
struct Wrap;
template <typename T, typename R, typename ... Args, R(T::* F)(Args...)>
struct Wrap<F> {
    T obj_;

    Wrap<F>(T instance) : obj_(instance) {}

    // it seems you don't take care of the cvref qualifiers, so I won't write all the overloads.
    auto operator()(Args... args) {
        return (obj_.*F)(args...);
    }
    //auto operator()(T* obj, Args... args) {
    //    return (obj->*F)(args...);
    //}
};

//template <auto F>
//struct WrapP;
//
//template <typename T, typename R, typename ... Args, R(T::* F)(Args...)>
//struct WrapP<F> {
//    T* obj_;
//
//    WrapP(T* instance) : obj_(instance) {}
//
//    auto operator()(Args... args) {
//        return (obj_->*F)(args...);
//    }
//};

struct foo {
    int bar(double) { 
        return 314; };
};

template <auto X>
auto makexx(decltype(X) x) {
    return Wrap<decltype(X)>{ x };
}

int main() {
    foo x;
    x.bar(3.1415);

    Wrap<&foo::bar> f{ x };
    std::cout << f(3.14159265) << std::endl;

    smack::util::WrapP<&foo::bar> fp{ &x };
    std::cout << fp( 3.14159265  ) << std::endl;

    return 0;
}

class Some
{
public:
    void do_something(int value, double amount) {
        cout << "value=" << value << " amount=" << amount << endl;
    }

    void do_something_else(std::string const& first, double& second, int third) {
        cout << "first=" << first << " second=" << second << " third=" << third << endl;
    }

    int execute(const std::vector<std::string>& argv) {
        // Editor moans, compiler cool.
        //Wrapper<decltype(&do_something)> obj{}; //Should be able to deduce Args to be [int, double]
        //// Editor moans, compiler cool.
        //obj(5, 17.4); //Would call do_something(5, 17.4);
        //Wrapper<free_function> obj2; //Should be able to deduce Args to be [std::string const&, double&, int]
        //// Editor moans, compiler cool.
        //obj2(313); //Would call do_something_else("Hello there!", value, 70);

        return 0;
    }
};

//int main( int argc, char**argv )
//{
//    Some some;
//    using namespace std::placeholders;  // for _1, _2, _3...
//    auto w = std::bind(
//        &Some::do_something,
//        &some,
//        // Don't want this.
//        _1, _2 );
//    w( 313, 3.14159265);
//    return 0;
//
//    std::vector<std::string> cmdArgv(
//        argv + 1,
//        argv + argc);
//
//    return some.execute(cmdArgv);
//}

#else 

#include <iostream>

void free_funct(int p1, double p2) {
    std::cout << "p1=" << p1 << " p2=" << p2 << endl;
}

struct object {
    void object_funct(double p1, int p2) const {
        std::cout << "p1=" << p1 << " p2=" << p2 << endl;
    }
    int exec(double p1, int p2) {
        // Ultimate goal:
        // Wrapper<(???::object_funct> functor3;
        // functor_3(p1, p2;)
    }
};

template <auto F>
class Wrapper {};

template <typename R, typename... Args, auto (F)(Args...)->R>
struct Wrapper<F>
{
    auto operator()(Args... args) const -> R {
        return F(args...);
    }
};

int main() {
    // Free funct.
    Wrapper<free_funct> functor1{};
    functor1(313, 3.141592);
    object object;
    // Extrinsic call to member function.
    object.object_funct(3.1415926, 313);
    // Problem 1:
    Wrapper<&object::object_funct> functor2;
    // How to express this below
    // functor2(3, 1415926, 313);
    object.exec(2.718281, 121);
    return 0;
}

#endif
