/*
 * $Id: 5d9b36a7883830a97f9c90d57b644e02f8fba503 $
 *
 * Console application helper.
 *
 * Copyright (c) 2019-2021 Michael Binz
 */

#pragma once

#include <array>
#include <cstddef>
#include <cstdlib>
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
namespace cli {

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::initializer_list;
using std::size_t;

/**
 * Define the transformation function.  Implementations for primitives 
 * and string-like types are available in the implementation file.
 */
template <typename T>
void transform(const char* in, T& out);

using cstr = const char*;

template<int N>
struct Choice: Choice<N-1>
{};

template<> struct Choice<0>
{};

template <typename T, std::enable_if_t<std::is_reference<T>::value, bool> = true>
constexpr cstr get_typename_(Choice<6>) {
    using i1 = typename std::remove_reference<T>::type;
    using i2 = typename std::remove_const<i1>::type;

    return get_typename_<i2>(Choice<4>{});
}

template <typename T, std::enable_if_t<std::is_pointer<T>::value, bool> = true>
constexpr cstr get_typename_(Choice<5>) {
    using i1 = typename std::remove_pointer<T>::type;
    using i2 = typename std::remove_const<i1>::type;
    using i3 = typename std::add_pointer<i2>::type;

    return get_typename_<i3>(Choice<4>{});
}

template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
constexpr cstr get_typename_(Choice<4>) {
    const auto digits = std::numeric_limits<T>::digits;

    if ( digits == 1 )
        return "bool";

    if ( sizeof( T ) == 1 ) {
            return std::numeric_limits<T>::min() == 0 ?
                "ubyte" : "byte";
    }
    if ( sizeof( T ) == 2 ) {
            return std::numeric_limits<T>::min() == 0 ?
                "ushort" : "short";
    }
    if ( sizeof( T ) == 4 ) {
            return std::numeric_limits<T>::min() == 0 ?
                "uint" : "int";
    }

    return std::numeric_limits<T>::min() == 0 ?
        "ulong" : "long";
}

template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
constexpr cstr get_typename_(Choice<3>) {
    const auto digits = std::numeric_limits<T>::digits;

    if ( digits <= std::numeric_limits<float>::digits )
        return "float";
    if ( digits <= std::numeric_limits<double>::digits )
        return "double";

    return "ldouble";
}

template<typename T,
std::enable_if_t<std::is_same<T, std::string>::value, bool> = true>
constexpr cstr get_typename_(Choice<2>) {
    return "string"; 
}

template<typename T,
std::enable_if_t<std::is_same<T, char*>::value, bool> = true>
constexpr cstr get_typename_(Choice<1>) {
    return "string"; 
}

template<typename T>
constexpr cstr get_typename_() {
    return "whatever"; 
}

template<typename T>
constexpr cstr get_typename_(Choice<0>) {
    return get_typename_<T>(); 
}

/**
 * @return a string representation for the supported types.
 */
template<typename T>
constexpr cstr get_typename() { 
    return get_typename_<T>(Choice<10>{}); 
}

/**
 * @return a string representation for the supported types.
 */
template<typename T>
constexpr cstr get_typename( T type ) { 
    return get_typename<decltype(type)>(); 
}

template<typename R, size_t I = 0, typename Func, typename ...Ts>
typename std::enable_if<I == sizeof...(Ts), std::array<R, sizeof...(Ts)>>::type
map_tuple(std::tuple<Ts...>&, Func) {
    std::array<R, sizeof...(Ts)> result;
    return result;
}

template<typename R, size_t I = 0, typename Func, typename ...Ts>
typename std::enable_if<I != sizeof...(Ts), std::array<R, sizeof...(Ts)>>::type
map_tuple(std::tuple<Ts...>& tpl, Func func) {
    std::array<R, sizeof...(Ts)> result =
        map_tuple<R, I + 1>(tpl, func);
    result[I] =
        func(std::get<I>(tpl));
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
            // If the passed command is empty all lines are printed.
            // Otherwise prints only the lines that contain the passed command
            // name.
            if (c.empty() || c.find( command ) == 0)
                cerr << c << endl;
        }
    }

public:
    CliApplication(Cs ... commands) :
        commands_(commands...) {
    }

    /**
     * Launch the application using the passed arguments.  Note that the 
     * passed arguments must not include the name of the application. Do
     * not pass argv[0].  See and prefer launch( int, char** ) which directly
     * accepts the arguments received in a main()-function.
     */
    int launch(const std::vector<string>& argv) {
        if (argv.empty()) {
            cerr << "No arguments. Available commands:" << endl;
            printHelp();
            return EXIT_FAILURE;
        }
        else if (argv.size() == 1 && argv[0] == "?") {
            printHelp();
            return EXIT_SUCCESS;
        }

        // Take the command name.
        const string& cmd_name =
            argv[0];
        // And the parameter names, excluding the command name.
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

    /**
     * Launch the application using the passed arguments.  Just forward the arguments
     * that were passed to main().
     * 
     * @param argc The argument count, as defined by the C/C++ main()-function.
     * @param argc The arguments, as defined by the C/C++ main()-function.
     */
    int launch(int argc, char** argv) {
        // Skip the program name.
        std::vector<std::string> cmdArgv(
            argv + 1,
            argv + argc);

        return launch(cmdArgv);
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
    R callFunc(VT& params, std::integer_sequence<int, S...>) const {
        return operator()(std::get<S>(params) ...);
    }

    initializer_list<const char*> parameterHelp_;

    /**
     * Trigger mapping.
     */
    template <typename ... T>
    void map(T ...) const {
    }

    template <typename T>
    int tf(T& param, const string& str) const {
        try {
            transform(str.c_str(), param);
        }
        catch (std::invalid_argument&) {
            std::stringstream s;
            s << 
                std::quoted(str) <<
                " -> " << 
                get_typename<T>();

            throw std::invalid_argument(s.str());
        }
        return 0;
    }

    template <typename V, int ... S>
    void updateImpl(
        VT& params,
        const V& v,
        const std::integer_sequence<int, S...>&) const {
        if (v.size() != kParameterCount) {
            throw std::invalid_argument("Bad array size.");
        }
        map(
            tf(std::get<S>(params), v[S]) ...
        ); 
   }

public:
    Command(
        const string& name,
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
    R operator()(Args... a) const {
        return func_(a...);
    }

    /**
     * Call the command with arguments to be converted.  Note that
     * the number of parameters represents the actual number of offered
     * command parameters, not including the command name or other stuff.
     */
    R operator()(const std::vector<string>& v) const {
        if (v.size() != kParameterCount) {
            throw std::invalid_argument("Wrong number of parameters.");
        }

        VT params;

        constexpr auto integer_sequence = 
            std::make_integer_sequence<int, kParameterCount>{};

        updateImpl(
            params,
            v,
            integer_sequence);

        return callFunc(
            params,
            integer_sequence);
    }

    /**
     * Creates a single-line command description that is displayed
     * in the generated cli help.
     */
    string to_string() const {
        // Get the raw type names of the parameters.
#if 0
        std::array<string, kParameterCount>
            expander{ (get_typename<Args>()) ... };
#else
        VT tup;

        std::array<string, kParameterCount> expander = map_tuple<string>(
            tup,
            [](auto t) {
                return get_typename( t );
            }
        );
#endif
        // If help was passed prepend the raw types with the 
        // passed display names.
        size_t idx = 0;
        for (string c : parameterHelp_) {
            if (c.empty())
                continue;
            expander[idx] = c + ":" + expander[idx];
            ++idx;
        }

        // Line starts with the command name.
        string result{ name_ };

        if (!expander.size())
            return result;

        result.append(
            " ");
        // Add the first argument.
        result.append(
            expander[0]);
        // For the remaining arguments.
        for (size_t i = 1; i < expander.size(); i++) {
            result.append(
                ", ");
            result.append(
                expander[i]);
        }

        return result;
    }

    string get_name() const {
        return name_;
    }
};

/**
 * The required template specialisations used to create a functor for
 * a callable entity.
 */
template <auto F>
class PListDed {};

/**
 * Specialisation for free functions. 
 */
template <typename R, typename... Args, auto (F)(Args...)->R>
class PListDed<F> {

public:
    template <typename Fu>
    static auto make(
        string name,
        Fu function,
        initializer_list<const char*> parameterHelper)
    {
        auto functor =
            [function](Args ... a) {
            return function(a...);
        };

        Command<decltype(functor), Args ...>
            result(name, functor, parameterHelper);
        return result;
    }
};

/**
 * Specialisation for instance operations.
 */
template <typename T, typename R, typename ... Args, R(T::* F)(Args...)>
class PListDed<F> {
public:
    template <typename Ty>
    static auto make(
        const Ty instance,
        string name,
        initializer_list<const char*> parameterHelper = {})
    {
        auto functor =
            [instance](Args ... a) {
            return (instance->*F)(a...);
        };

        Command<decltype(functor), Args ...>
            result(name, functor, parameterHelper);
        return result;
    }
};

/**
 * Specialisation for const instance operations.
 */
template <typename T, typename R, typename ... Args, R(T::* F)(Args...) const>
class PListDed<F> {
public:
    template <typename Ty>
    static auto make(
        const Ty instance,
        string name,
        initializer_list<const char*> parameterHelper = {})
    {
        auto functor =
            [instance](Args ... a) {
            return (instance->*F)(a...);
        };

        Command<decltype(functor), Args ...>
            result(name, functor, parameterHelper);
        return result;
    }
};

/**
 * Offers the external interface.
 */
struct Commands {
    /**
     * Create a command for a free function.
     * 
     * @param F The function reference.
     * @param name The name of the resulting command.
     * @param parameterHelper An alternative name for each parameter.  This is optional,
     * if it is not passed, then the raw typename is displayed in the generated help
     * page.  If it is passed its length has to correspond to the number of parameters
     * of the referenced operation.
     */
    template <auto F>
    static auto make(
        string name,
        initializer_list<const char*> parameterHelper = {})
    {
        return PListDed<F>::make(
            name,
            F,
            parameterHelper
        );
    }

    /**
     * Create a command for a member function.
     *
     * @param F The operation reference.
     * @param T The type of the class implementing F.  This is deduced from the
     * \p instance parameter.
     * @param name The name of the resulting command.
     * @param instance The instance to use when calling the operation.
     * @param parameterHelper An alternative name for each parameter.  This is optional,
     * if it is not passed, then the raw typename is displayed in the generated help
     * page.  If it is passed its length has to correspond to the number of parameters
     * of the referenced operation.
     */
    template <auto const F, typename T>
    static auto make(
        string name,
        const T instance,
        initializer_list<const char*> parameterHelper = {})
    {
        return PListDed<F>::template make<T>(
            instance,
            name,
            parameterHelper
            );
    }
};

} // namespace cli
} // namespace smack
