/*
 * $Id: 5d9b36a7883830a97f9c90d57b644e02f8fba503 $
 *
 * Console application helper.
 *
 * Copyright (c) 2019-2020 Michael Binz
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdlib>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace smack {
namespace util {

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::initializer_list;
using std::size_t;

// Produces the necessary transformations for the char* types
// as well as for std::string.
template <typename T>
void transform(const char* in, T& out) {
    out = in;
}

void transform(const char* in, int& out);
void transform(const char* in, long& out);
void transform(const char* in, float& out);
void transform(const char* in, double& out);
void transform(const char* in, bool& out);

/**
 * @return a string representation for the supported types.
 */
template <typename T>
constexpr const char* resolve_type() {
    using C = char*;

    if (typeid(bool) == typeid(T))
        return "bool";
    else if (typeid(short) == typeid(T))
        return "short";
    else if (typeid(int) == typeid(T))
        return "int";
    else if (typeid(long) == typeid(T))
        return "long";
    else if (typeid(double) == typeid(T))
        return "double";
    else if (typeid(float) == typeid(T))
        return "float";
    else if (typeid(C) == typeid(T))
        return "string";
    else if (typeid(const char*) == typeid(T))
        return "string";
    else if (typeid(string) == typeid(T))
        return "string";
    else if (typeid(std::experimental::filesystem::path) == typeid(T))
        return "filename";

    return typeid(T).name();
}

template<typename R, size_t I = 0, typename Func, typename ...Ts>
typename std::enable_if<I == sizeof...(Ts),std::array<R, sizeof...(Ts)>>::type
map_tuple(std::tuple<Ts...> &, Func) {
    std::array<R, sizeof...(Ts)> result;
    return result;
}

template<typename R, size_t I = 0, typename Func, typename ...Ts>
typename std::enable_if<I != sizeof...(Ts), std::array<R, sizeof...(Ts)>>::type
map_tuple(std::tuple<Ts...> & tpl, Func func) {
    R c = func(std::get<I>(tpl));
    std::array<R, sizeof...(Ts)> result = map_tuple<R, I + 1>(tpl, func);
    result[I] = c;
    return result;
}

/**
 * Represents the Cli.
 * Use makeCliApplication(...) to create an instance.
 */
template <typename... Cs>
class CliApplication
{
    std::tuple<Cs...> commands_;

    bool found_{};

    template <size_t I>
    typename std::enable_if<I == sizeof...(Cs), int>::type
    find(const string& name, const std::vector<string>& argv) {
        if (found_) {
            cerr << 
                "Command '" <<
                name <<
                "' does not support " <<
                std::to_string( argv.size() ) <<
                " parameters." <<
                endl;
            printHelp(name);
        }
        else
        {
            cerr << "Unknown command '" << name << "'." << endl;
            printHelp();
        }
        return EXIT_FAILURE;
    }
    template <size_t I>
    typename std::enable_if<I != sizeof...(Cs), int>::type
    find(const string& name, const std::vector<string>& argv) {
        auto c = std::get<I>(commands_);
        if (name == c.get_name() && argv.size() == c.kParameterCount)
            return c(argv);
        else if (name == c.get_name()) {
            found_ = true;
        }
        return find<I + 1>(name, argv);
    }

    void printHelp( const string& command = "" ) {
        std::array<string, sizeof ... (Cs)> x = map_tuple<string>(
            commands_,
            [](auto t) {
            return t.to_string();
        });

        for (const string& c : x) {
            if (c.empty() || c.find( command ) == 0)
                cerr << c << endl;
        }
    }

public:
    CliApplication(Cs ... commands) :
        commands_(commands...) {
    }

    int launch(std::vector<string> argv) {
        if (argv.empty()) {
            cerr << "No arguments. Available commands:" << endl;
            printHelp();
            return EXIT_FAILURE;
        }
        else if (argv.size() == 1 && argv[0] == "?") {
            printHelp();
            return EXIT_SUCCESS;
        }

        const string& cmd_name =
            argv[0];

        std::vector<string> cmdArgv(
            argv.begin() + 1,
            argv.end());

        try {
            return find<0>(
                cmd_name,
                cmdArgv );
        }
        catch (std::invalid_argument& e) {
            cout << "Conversion failed: " << e.what() << endl;

            return EXIT_FAILURE;
        }
    }
};

/**
 * Create a CliApplication instance.
 */
template <typename... Cs>
auto makeCliApplication(Cs ... commands) {
    CliApplication < decltype(commands) ... > result(commands ...);
    return result;
}

/**
 * A single command.  This wraps a function and the necessary logic
 * to map from string-based command line arguments.
 */
template <typename F, typename ... Args>
class Command {
    // Defines the return type.
    using R = int;
    // Intermediate storage for the converted arguments.
    using VT =
        std::tuple< typename std::decay<Args>::type ... >;

    /**
     * The command's name.
     */
    string name_;

    /**
     * The function to be called.
     */
    F func_;

public:
    static constexpr size_t kParameterCount{
        sizeof ... (Args)
    };

private:
    template<int ... S>
    R callFunc(VT& params, std::integer_sequence<int, S...>) {
        return operator()(std::get<S>(params) ...);
    }

    initializer_list<const char*> parameterHelp_;

    /**
     * Trigger mapping.
     */
    template <typename ... TT>
    void map(TT ...) {
    }

    template <int I>
    int tf(VT& params, const string& str) {
        try {
            transform(str.c_str(), std::get<I>(params));
        }
        catch (std::invalid_argument&) {
            std::stringstream s;
            s << 
                std::quoted(str) <<
                " -> " << 
                resolve_type<decltype(
                    std::get<I>(params))>();

            throw std::invalid_argument(s.str());
        }
        return I;
    }

    template <int ... S>
    void updateImpl(
        VT& params,
        const std::vector<string>& v,
        const std::integer_sequence<int, S...>&) {
        if (v.size() != kParameterCount) {
            throw std::invalid_argument("Bad array size.");
        }
        map(
            tf<S>(params, v[S]) ...
        );
    }

public:
    Command(
        string name,
        F f,
        initializer_list<const char*> parameterHelp = {})
        :
        name_(name),
        func_(f),
        parameterHelp_(parameterHelp)
    {
        if ( parameterHelp.size() > parameterHelp_.size() )
            throw std::invalid_argument("Too many parameter help strings.");
    }

    /**
     * Supports a typesave call of the command.
     */
    R operator()(Args... a) {
        return func_(a...);
    }

    /**
     * Call the command with arguments to be converted.  Note that
     * the number of parameters represents the actual number of offered
     * command parameters, not including the command name or other stuff.
     */
    R operator()(const std::vector<string>& v) {
        if (v.size() != kParameterCount) {
            throw std::invalid_argument("Wrong number of parameters.");
        }

        VT params;

        const auto integer_sequence = 
            std::make_integer_sequence<int, kParameterCount>{};

        updateImpl(
            params,
            v,
            integer_sequence);

        return callFunc(
            params,
            integer_sequence);
    }

    string to_string() const {
        return name_ + " " + type_name();
    }

    constexpr string type_name() const {
        std::array<const char*, kParameterCount>
            expander{ (resolve_type<Args>()) ... };

        size_t idx = 0;
        for (auto c : parameterHelp_)
            expander[idx++] = c;

        string result;

        if (!expander.size())
            return result;

        string comma(", ");

        result = expander[0];

        for (size_t i = 1; i < expander.size(); i++) {
            result.append(
                comma);
            result.append(
                expander[i]);
        }

        return result;
    }

    string get_name() const {
        return name_;
    }
};

template <typename ... Args>
class Commands {
    template <typename H, typename F>
    static auto make_(H& host, F member) {
        return [&host, member](Args ... a) mutable {
            return (host.*(member))(a...);
        };
    }

    template <typename F>
    static auto make_(F member) {
        return [member](Args ... a) {
            return member(a...);
        };
    }

public:
    template <typename H, typename F>
    static auto make(
        string name,
        H& host,
        F member,
        initializer_list<const char*> parameterHelper = {})
    {
        auto functor =
            make_(host, member);
        Command<decltype(functor), Args ...>
            result(name, functor, parameterHelper);
        return result;
    }
    template <typename F>
    static auto make(
        string name,
        F function,
        initializer_list<const char*> parameterHelper = {})
    {
        auto functor =
            make_(function);
        Command<decltype(functor), Args ...>
            result(name, functor, parameterHelper);
        return result;
    }
};

// https://stackoverflow.com/questions/46533698/how-to-deduce-argument-list-from-function-pointer
// https://stackoverflow.com/questions/6547056/template-parameter-deduction-with-function-pointers-and-references

/**
 * Used for template function argument deduction.
 * Use xxx below as a handier interface.
 */
template <auto F>
class ParameterListDed {};

template <typename Ret, typename... Args, auto (F)(Args...)->Ret>
class ParameterListDed<F> {
    template <typename H, typename F>
    static auto make_(H& host, F member) {
        return [&host, member](Args ... a) mutable {
            return (host.*(member))(a...);
        };
    }

    template <typename F>
    static auto make_(F member) {
        return [member](Args ... a) {
            return member(a...);
        };
    }

public:
    //template <typename H, typename F>
    //static auto make(
    //    string name,
    //    H& host,
    //    F member,
    //    initializer_list<const char*> parameterHelper = {})
    //{
    //    auto functor =
    //        make_(host, member);
    //    Command<decltype(functor), Args ...>
    //        result(name, functor, parameterHelper);
    //    return result;
    //}
    template <typename F>
    static auto make(
        string name,
        F function,
        initializer_list<const char*> parameterHelper)
    {
        auto functor =
            make_(function);
        Command<decltype(functor), Args ...>
            result(name, functor, parameterHelper);
        return result;
    }
};

/**
 * Offers the external interface.
 */
struct Outer {
    /**
     * Create a command for a free function.
     */
    template <auto F>
    static auto make(
        string name,
        initializer_list<const char*> parameterHelper = {})
    {
        return ParameterListDed<F>::make(
            name,
            F,
            parameterHelper
        );
    }
};

} // namespace util
} // namespace smack
