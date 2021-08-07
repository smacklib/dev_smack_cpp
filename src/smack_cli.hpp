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
#include <stdexcept>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace smack::cli {

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::initializer_list;
using std::size_t;

class conversion_failure : public std::runtime_error {
public:
    using base = std::runtime_error;
    explicit conversion_failure(const string& msg) : base(msg.c_str()) {}
    explicit conversion_failure(const char* msg) : base(msg) {}
};
class command_not_found : public std::runtime_error {
public:
    using base = std::runtime_error;
    explicit command_not_found(const string& msg) : base(msg.c_str()) {}
    explicit command_not_found(const char* msg) : base(msg) {}
};
class command_args_incompatible : public std::runtime_error {
public:
    using base = std::runtime_error;
    explicit command_args_incompatible(const string& msg) : base(msg.c_str()) {}
    explicit command_args_incompatible(const char* msg) : base(msg) {}
};

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
    return "unknown"; 
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

template<typename R, typename T, typename Func, auto ... I>
auto map_tuple_(T& tpl, Func func, std::index_sequence<I...> ) {
    std::array<R, sizeof ... (I)> result{
        func(std::get<I>(tpl)) ... 
    };

    return result;
}

template<typename R, typename T, typename Func>
auto map_tuple(T& tpl, Func func) {
    constexpr auto i = 
        std::make_index_sequence<std::tuple_size_v<T>>{};

    return map_tuple_<R>( tpl, func, i );
}

/**
 * A single command.  This wraps a function and the necessary logic
 * to map from string-based command line arguments.
 */
class Command {
    // Defines the return type.
    using R = int;

    /**
     * The command's name.
     */
    string name_;

    /**
     * The number of arguments the command accepts.
     */
    size_t argumentCount_;

    /**
     * The function to be called.
     */
    std::function<R(const std::vector<string>&)> func_;

    /**
     * A help line describing the command.
     */
    string helpLine_;

public:
    Command(
        const string& name,
        size_t argumentCount,
        std::function<R(const std::vector<string>&)> f,
        const string& helpLine)
        :
        name_(name),
        argumentCount_(argumentCount),
        func_(f),
        helpLine_(helpLine)
    {
    }

    /**
     * Call the command with arguments to be converted.  Note that
     * the number of parameters represents the actual number of offered
     * command parameters, not including the command name or other stuff.
     */
    R callv(const std::vector<std::string>& v) const {
        if (v.size() != argumentCount_)
            throw std::invalid_argument("Wrong number of arguments.");

        return func_( v );
    }

    /**
     * Call the command with arguments to be converted.  Note that
     * the number of parameters represents the actual number of offered
     * command parameters, not including the command name or other stuff.
     */
    template <typename T = string, typename ... V>
    R call(V const & ... argv) const 
    {
        std::vector<T> va {
            argv ... 
        };

        return callv( va );
    }

    /**
     * Creates a single-line command description that is displayed
     * in the generated cli help.
     */
    string to_string() const 
    {
        return helpLine_;
    }

    string get_name() const {
        return name_;
    }

    constexpr size_t get_argument_count() const {
        return argumentCount_;
    }
};

struct internal {
    using R = int;
    using I = string;
    using IT = const std::vector<I>&;

    /**
     * Make a parameter pack from the passed params tuple and
     * call the functor.
     */
    template<typename Fu,typename Tp, auto ... S>
    static R callFuncImpl(Fu f, const Tp& params, std::index_sequence<S...>) {
        return f(std::get<S>(params) ...);
    }
    /**
     * Make a parameter pack from the passed params tuple and
     * call the functor.
     */
    template<typename Fu, typename Tp>
    static R callFunc(Fu f, const Tp& params) {
        constexpr auto sz = std::tuple_size_v<Tp>;

        constexpr auto idx = 
            std::make_index_sequence<sz>{};

        return callFuncImpl( f, params, idx );
    }

    /**
     * Trigger mapping.
     */
    template <typename ... T>
    static void map(T ...) {
    }

    template <typename T>
    static int tf(T& param, const string& str) {
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

    template <typename T, typename Tp, auto ... S>
    static void convertImpl(
        const T& v,
        Tp& params,
        const std::index_sequence<S...>&)
    {
        map(
            tf(std::get<S>(params), v[S]) ...
        ); 
    }

    template <typename Tp>
    static void convert(
        Tp& params,
        const std::vector<std::string>& argv) 
    {
        constexpr auto sz = std::tuple_size_v<Tp>;

        if (argv.size() != sz)
            throw std::invalid_argument("Wrong number of arguments.");
        
        constexpr auto idx =
            std::make_index_sequence<sz>{};

        convertImpl(argv,params,idx);
    }

    /**
     * Creates a single-line command description that is displayed
     * in the generated cli help.
     */
    template <typename Tp>
    static string make_help_string(
        const std::string& name, 
        initializer_list<const char*> parameterHelp)
    {
        // Get the raw type names of the parameters.
        Tp tup;

        std::array<string, std::tuple_size_v<Tp>> expander = map_tuple<string>(
            tup,
            [](auto t) {
                return get_typename(t);
            }
        );

        // If help was passed prepend the raw types with the
        // passed display names.
        size_t idx = 0;
        for (string c : parameterHelp) {
            if (c.empty())
                continue;
            expander[idx] = c + ":" + expander[idx];
            ++idx;
        }

        // Line starts with the command name.
        string result{ name };

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
};

/**
 * The required template specialisations used to create a functor for
 * a callable entity.
 */
template <auto F>
struct PListDed {};

/**
 * Specialisation for free functions. 
 */
template <typename R, typename... Args, auto (F)(Args...)->R>
struct PListDed<F> 
{
    static auto make(
        const string& name,
        initializer_list<const char*> parameterHelper)
    {
        using Tp =
            std::tuple< typename std::decay<Args>::type ... >;

        auto cvf = [](internal::IT v){
            Tp params;
            internal::convert( params, v );
            return internal::callFunc(F, params);
        };

        string help = internal::make_help_string<Tp>(name,parameterHelper);

        Command
            result(name, std::tuple_size_v<Tp>, cvf, help);

        return result;
    }
};

/**
 * Specialisation for instance operations.
 */
template <typename T, typename R, typename ... Args, R(T::* F)(Args...)>
struct PListDed<F> 
{
    template <typename Ty>
    static auto make(
        const Ty instance,
        const string& name,
        initializer_list<const char*> parameterHelper = {})
    {
        auto functor =
            [instance](Args ... a) {
            return (instance->*F)(a...);
        };

        using Tp =
            std::tuple< typename std::decay<Args>::type ... >;

        auto cvf = [functor](internal::IT v){
            Tp params;
            internal::convert(params, v);
            return internal::callFunc(functor, params);
        };

        string help = internal::make_help_string<Tp>(name,parameterHelper);

        Command
            result(name, std::tuple_size_v<Tp>, cvf, help);

        return result;
    }
};

/**
 * Specialisation for const instance operations.
 */
template <typename T, typename R, typename ... Args, R(T::* F)(Args...) const>
struct PListDed<F> 
{
    template <typename Ty>
    static auto make(
        const Ty instance,
        const string& name,
        initializer_list<const char*> parameterHelper = {})
    {
        auto functor =
            [instance](Args ... a) {
            return (instance->*F)(a...);
        };

        using Tp =
            std::tuple< typename std::decay<Args>::type ... >;

        auto cvf = [functor](internal::IT v){
            Tp params;

            internal::convert( params, v );

            return internal::callFunc(functor, params);
        };

        string help = internal::make_help_string<Tp>(name,parameterHelper);

        Command
            result(name, std::tuple_size_v<Tp>, cvf, help);

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
        const string& name,
        initializer_list<const char*> parameterHelper = {})
    {
        return PListDed<F>::make(
            name,
            parameterHelper );
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
        const string& name,
        const T instance,
        initializer_list<const char*> parameterHelper = {})
    {
        return PListDed<F>::template make<T>(
            instance,
            name,
            parameterHelper );
    }
};

/**
 * Represents the Cli.
 */
template <typename... Cs>
class CliApplication
{
    /**
     * Maps a command name to a map of n-argument alternatives. 
     */
    std::map<string, std::map<size_t, Command>> commandMap_;

    /**
     * Print the help page for the application.
     * @param stream The target stream help is written to.
     * @param command A command name used to print only
     * the command's alternatives.
     */
    void printHelp(
        std::ostream& stream,
        const string& command = "" )
    {
        if (command.empty()) {

            for (auto names : commandMap_)
                for (auto argcounts : names.second) {
                    stream << argcounts.second.to_string() << "\n";
                }

            return;
        }

        if (! command.empty() &&  commandMap_.count(command) == 0) {
            stream << "Implementation error. Unknown command: " << command << "\n";
            return;
        }

        auto names = commandMap_.at(command);

        for (auto argcounts : names)
            stream << argcounts.second.to_string() << "\n";
    }

public:
    /**
     * Create an instance that offers the passed commands.
     */
    CliApplication(const Cs& ... commands) {

        std::vector<Command> va{
            commands ...
        };

        for (Command c : va) {
            auto cname = 
                c.get_name();
            auto ccount = 
                c.get_argument_count();

            // Ensure that no duplicate commands are added.
            if ( commandMap_.count( cname ) == 1 && 
                commandMap_.at(cname).count(ccount) == 1 )
            {
                cerr <<
                    "Implementation error: Duplicate definition of command " <<
                    std::quoted(cname) <<
                    " with argument count: " <<
                    ccount;

                std::exit(EXIT_FAILURE);
            }

            commandMap_[c.get_name()].insert({ c.get_argument_count(), c });
        }
    }

    /**
     * Launch the application using the passed arguments.  Note that the 
     * passed arguments must not include the name of the application. Do
     * not pass argv[0].  See and prefer launch( int, char** ) which directly
     * accepts the arguments received in a main()-function.
     */
    int launch(const std::vector<string>& argv) noexcept {
        if (argv.empty()) {
            cerr << "No arguments. Available commands:" << endl;
            printHelp(cerr);
            return EXIT_FAILURE;
        }
        else if (argv.size() == 1 && argv[0] == "?") {
            printHelp(cout);
            return EXIT_SUCCESS;
        }

        // Take the command name.
        const string& cmd_name =
            argv[0];
        // And the parameter names, excluding the command name.
        std::vector<string> cmdArgv(
            argv.begin() + 1,
            argv.end());

        if (commandMap_.count(cmd_name) == 0) {
            cerr << "Unknown command '" << cmd_name << "'.\n";
            cerr << "Supported commands are:\n";
            printHelp(cerr);
            return EXIT_FAILURE;
        }

        auto commands = commandMap_[cmd_name];
        if (commands.count(cmdArgv.size()) == 0) {
            cerr <<
                "The command '" <<
                cmd_name <<
                "' does not support " <<
                std::to_string(cmdArgv.size()) <<
                " parameters.\n";
            cerr << "Supported:\n";
            printHelp(cerr, cmd_name);
            return EXIT_FAILURE;
        }

        try {
            return commands.at(cmdArgv.size()).callv(cmdArgv);
        }
        catch (const conversion_failure& e) {
            cerr << 
                "Conversion failed: " << 
                e.what() <<
                endl;
        }
        catch (const std::exception &e) {
            cerr <<
                cmd_name <<
                " failed: " <<
                e.what() <<
                endl;
        }

        return EXIT_FAILURE;
    }

    /**
     * Launch the application using the passed arguments.  Just forward the arguments
     * that were passed to main().
     * 
     * @param argc The argument count, as defined by the C/C++ main()-function.
     * @param argc The arguments, as defined by the C/C++ main()-function.
     */
    int launch(int argc, const char* const* argv) noexcept {
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
[[deprecated("Directly construct an instance of smack::cli::CliApplication.")]]
auto makeCliApplication(const Cs& ... commands) {
    CliApplication result(commands ...);
    return result;
}

} // namespace smack::cli
