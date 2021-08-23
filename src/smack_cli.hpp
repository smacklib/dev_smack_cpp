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
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <limits>
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
template <typename From, typename To>
void transform(From in, To& out);

template <typename To>
void transform(const std::string& in, To& out) {
    transform(in.c_str(), out);
}

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
std::enable_if_t<std::is_same<T, string>::value, bool> = true>
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

namespace internal {
    using I = string;
    using IT = const std::vector<I>&;

    static string EMPTY_STRING;

    /**
     * The quote character to be used in error messages.
     */
    constexpr char kDelim_ {'\''};

    /**
     * Make a parameter pack from the passed params tuple and
     * call the functor.
     */
    template<typename Fu,typename Tp, auto ... S>
    static auto callFuncImpl(Fu f, const Tp& params, std::index_sequence<S...>) {
        return f(std::get<S>(params) ...);
    }
    /**
     * Make a parameter pack from the passed params tuple and
     * call the functor.
     */
    template<typename Fu, typename Tp>
    static auto callFunc(Fu f, const Tp& params)
    {
        constexpr auto sz =
            std::tuple_size_v<Tp>;
        constexpr auto idx = 
            std::make_index_sequence<sz>{};
        return callFuncImpl( f, params, idx );
    }

    /**
     * Trigger mapping.
     */
    template <typename ... T>
    static void map(T ...) 
    {
    }

    template <typename To, typename From>
    static int tf(To& param, const From& str)
    {
        try {
            transform(str, param);
        }
        catch (std::invalid_argument&) {
            std::stringstream s;
            s << 
                std::quoted(str, kDelim_) <<
                " -> " << 
                get_typename<To>();

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
        const std::vector<string>& argv) 
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
        const string& name,
        initializer_list<const char*> parameterHelp,
        const string& description = {} )
    {
        // Get the raw type names of the parameters.
        Tp tup;

        auto expander = map_tuple<string>(
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

        // Append the command arguments.
        if (expander.size() > 0) {
            result.append(
                " ");
            // Add the first argument.
            result.append(
                expander[0]);
            // For the remaining arguments.
            for (size_t i = 1; i < expander.size(); i++) {
                result.append(
                    " ");
                result.append(
                    expander[i]);
            }
        }

        // Add the description in the second line.
        if (!description.empty()) {
            result.append("\n    ");
            result.append(description);
        }

        return result;
    }

    template <typename ParameterTuple, typename T>
    static auto wrap( T functor )
    {
        return [functor](internal::IT v){
            ParameterTuple params;
            internal::convert( params, v );
            return internal::callFunc(functor, params);
        };
    }
};

/**
 * A single command.  This wraps a function and the necessary logic
 * to map from string-based command line arguments.
 */
class Command {
    // Defines the return type.
    using R = int;
    using I = string;
    using IT = const std::vector<I>&;

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
    std::function<R(IT)> func_;

    /**
     * A help line describing the command.
     */
    string helpLine_;

public:
    Command(
        const string& name,
        size_t argumentCount,
        std::function<R(IT)> f,
        const string& helpLine)
        :
        name_(name),
        argumentCount_(argumentCount),
        func_(f),
        helpLine_(helpLine)
    {
    }

    template <typename Tp, typename F>
    Command(
        const string& name,
        const string& description,
        initializer_list<const char*> parameterHelper,
        Tp& arguments,
        F f)
        :
        name_(name)
        , argumentCount_(std::tuple_size_v<Tp>)
        , func_( internal::wrap<Tp>(f) )
        , helpLine_( smack::cli::internal::make_help_string<Tp>(name,parameterHelper,description) )
    {
        std::cout << "Bah!\n";
    }

    /**
     * Call the command with arguments to be converted.
     */
    R callv(IT v) const {
        if (v.size() != argumentCount_)
            throw std::invalid_argument("Wrong number of arguments.");

        return func_( v );
    }

    /**
     * Call the command with arguments to be converted.
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

/**
 * Offers the external interface.
 */
class Commands {
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
            const string& description,
            initializer_list<const char*> parameterHelper)
        {
            using Tp =
                std::tuple< typename std::decay<Args>::type ... >;
            Tp t;
            return Command{name, description, parameterHelper, t, F};
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
            const string& description,
            initializer_list<const char*> parameterHelper = {})
        {
            auto functor =
                [instance](Args ... a) {
                return (instance->*F)(a...);
            };

            using Tp =
                std::tuple< typename std::decay<Args>::type ... >;
            auto cvf =
                internal::wrap<Tp>(functor);
            string help =
                internal::make_help_string<Tp>(name,parameterHelper,description);

            return Command{name, std::tuple_size_v<Tp>, cvf, help};
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
            const string& description,
            initializer_list<const char*> parameterHelper = {})
        {
            auto functor =
                [instance](Args ... a) {
                return (instance->*F)(a...);
            };

            using Tp =
                std::tuple< typename std::decay<Args>::type ... >;
            auto cvf = 
                internal::wrap<Tp>(functor);
            string help = 
                internal::make_help_string<Tp>(name, parameterHelper, description);

            return Command{name, std::tuple_size_v<Tp>, cvf, help};
        }
    };

public:
    /**
     * Create a command for a free function.
     *
     * @param F The function reference.
     * @param name The name of the resulting command.
     * @param parameterHelper An alternative name for each parameter.  This is
     * optional, if it is not passed, then the raw typename is displayed in the
     * generated help page.  If it is passed its length has to correspond to
     * the number of parameters of the referenced operation.
     */
    template <auto F>
    static auto make(
        const char* name,
        initializer_list<const char*> parameterHelper = {})
    {
        return PListDed<F>::make(
            name,
            internal::EMPTY_STRING,
            parameterHelper);
    }

    /**
     * Create a command for a free function.
     * 
     * @param F The function reference.
     * @param name The name of the resulting command.
     * @param description A textual description of the command printed in the
     * help page.
     * @param parameterHelper An alternative name for each parameter.  This is optional,
     * if it is not passed, then the raw typename is displayed in the generated help
     * page.  If it is passed its length has to correspond to the number of parameters
     * of the referenced operation.
     */
    template <auto F>
    static auto make(
        const char* name,
        const char* description,
        initializer_list<const char*> parameterHelper = {})
    {
        return PListDed<F>::make(
            name,
            description,
            parameterHelper);
    }

    /**
     * Create a command for a member function.
     *
     * @param F The operation reference.
     * @param T The type of the class implementing F.  This is deduced from the
     * \p instance parameter.
     * @param name The name of the resulting command.
     * @param instance The instance to use when calling the operation.
     * @param parameterHelper An alternative name for each parameter.  This is
     * optional, if it is not passed, then the raw typename is displayed in the
     * generated help page.  If it is passed its length has to correspond to
     * the number of parameters of the referenced operation.
     */
    template <auto const F, typename T>
    static auto make(
        const char* name,
        const T instance,
        initializer_list<const char*> parameterHelper = {})
    {
        return PListDed<F>::template make<T>(
            instance,
            name,
            internal::EMPTY_STRING,
            parameterHelper);
    }

    /**
     * Create a command for a member function.
     *
     * @param F The operation reference.
     * @param T The type of the class implementing F.  This is deduced from the
     * \p instance parameter.
     * @param name The name of the resulting command.
     * @param description A textual description of the command printed in the
     * help page.
     * @param instance The instance to use when calling the operation.
     * @param parameterHelper An alternative name for each parameter.  This is
     * optional, if it is not passed, then the raw typename is displayed in the
     * generated help page.  If it is passed its length has to correspond to
     * the number of parameters of the referenced operation.
     */
    template <auto const F, typename T>
    static auto make(
        const char* name,
        const char* description,
        const T instance,
        initializer_list<const char*> parameterHelper = {})
    {
        return PListDed<F>::template make<T>(
            instance,
            name,
            description,
            parameterHelper);
    }
};

/**
 * Represents the Cli.
 */
template <typename... Cs>
class CliApplication
{
    using CsTy_ = std::common_type_t<Cs...>;
    static_assert(std::is_same<Command, CsTy_>());

    /**
     * The registered commands. 
     */
    std::array<CsTy_, sizeof ... (Cs)> commands_;

    /**
     * Maps a command name to a map of n-argument alternatives.  The command entries
     * point to the entries in the commands_ array.
     */
    std::map<
        string,
        std::map<
            size_t,
            typename decltype(commands_)::const_pointer>> commandMap_;

    /**
     * The cli name.  This can be explicitly set using the operation set_name() or
     * is set implicitly if launch( argc, argv ) is used.
     */
    string name_;

    /**
     * A desription for this cli.
     */
    string description_;

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
            if (!name_.empty()) {
                // Show only the executable's file name, no
                // path, no extension.
                std::filesystem::path path{ name_ };
                path.replace_extension();

                stream << 
                    "Usage: " << 
                    path.filename().string() << 
                    " COMMAND arguments\n";
            }

            if (!description_.empty())
                cout << description_ << "\n";

            stream << "\nCommands:\n";

            for (auto names : commandMap_)
                for (auto argcounts : names.second) {
                    stream << argcounts.second->to_string() << "\n";
                }

            return;
        }

        if (! command.empty() &&  commandMap_.count(command) == 0) {
            stream << "Implementation error. Unknown command: " << command << "\n";
            return;
        }

        auto variants = commandMap_.at(command);
        for (auto variant : variants)
            stream << variant.second->to_string() << "\n";
    }

public:
    /**
     * Create an instance that offers the passed commands.
     * @param description The cli description that is displayed in the
     * help page.
     * @param commands The commands that are to be supported.
     */
    CliApplication(const char* description, const Cs& ... commands) :
        commands_{ commands ... },
        description_(description)
    {
        for (Command& c : commands_) {
            const auto& cname =
                c.get_name();
            const auto& ccount =
                c.get_argument_count();

            // Ensure that no duplicate commands are added.
            if (commandMap_.count(cname) == 1 &&
                commandMap_.at(cname).count(ccount) == 1)
            {
                cerr <<
                    "Implementation error: Duplicate definition of command " <<
                    std::quoted(cname, internal::kDelim_) <<
                    " with argument count: " <<
                    ccount;

                std::exit(EXIT_FAILURE);
            }

            commandMap_[cname][ccount] = &c;
        }
    }

    /**
     * Create an instance that offers the passed commands.
     * @param commands The commands that are to be supported.
     */
    CliApplication(const Cs& ... commands) : CliApplication("", commands ...)
    {
    }

    /**
     * Launch the application using the passed arguments.  Note that the 
     * passed arguments must not include the name of the application. Do
     * not pass argv[0].  See and prefer launch( int, char** ) which directly
     * accepts the arguments received in a main()-function.
     */
    int launch(const std::vector<string>& argv) noexcept
    {
        if (argv.empty()) {
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
            cerr << "Unknown command '" << cmd_name << "'.\n\n";
            printHelp( cerr );
            return EXIT_FAILURE;
        }

        auto commands = commandMap_[cmd_name];
        if ( commands.count( cmdArgv.size() ) == 0 ) {
            cerr <<
                "The command '" <<
                cmd_name <<
                "' does not support " <<
                std::to_string( cmdArgv.size() ) <<
                " parameters.\n";
            cerr << "Supported:\n";
            printHelp( cerr, cmd_name );
            return EXIT_FAILURE;
        }

        try {
            return commands.at( cmdArgv.size() )->callv( cmdArgv );
        }
        catch ( const conversion_failure& e ) {
            cerr << 
                "Conversion failed: " << 
                e.what() <<
                endl;
        }
        catch ( const std::exception &e ) {
            cerr <<
                std::quoted( cmd_name, internal::kDelim_ ) <<
                " failed: " <<
                e.what() <<
                endl;
        }
        catch ( ... ) {
            cerr <<
                std::quoted( cmd_name, internal::kDelim_ ) <<
                " failed unexpectedly." <<
                endl;
        }

        return EXIT_FAILURE;
    }

    /**
     * Launch the application using the passed arguments.  Just forward the arguments
     * that were passed to main().  If the application name was not explicitly set
     * then argv[0] is used as application name.
     * 
     * @param argc The argument count, as defined by the C/C++ main()-function.
     * @param argc The arguments, as defined by the C/C++ main()-function.
     */
    int launch(int argc, const char* const* argv) noexcept {
        if (name_.empty() && argc > 0)
            name_ = argv[0];

        // Skip the program name.
        std::vector<string> cmdArgv(
            argv + 1,
            argv + argc);

        return launch(cmdArgv);
    }

    /**
     * Get the name of the application.  This can be explicitly set to an
     * arbitrary value using the {@code set_name()}-operation.
     */
    string name() {
        return name_;
    }

    /**
     * Explicitly set the name of the application.
     */
    void set_name( const string& name ) {
        name_ = name;
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
