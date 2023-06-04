#include <string>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "components/endpoints/inspectors.hpp"
#include "components/endpoints/helpers.hpp"
#include "components/testcomponent.hpp"
#include "bindings/basic_logger/test_logger.hpp"
#include "cli.hpp"

using std::string;

using namespace sygaldry::bindings::cli::commands;
using namespace sygaldry::bindings::cli;

template <typename Cli>
string cli_process_input(Cli& cli, string input)
{
    cli.log.put.ss.str("");
    for (char c : input)
        cli.process(c);
    return cli.log.put.ss.str();
}

template<typename Config>
struct Echo
{
    static _consteval auto name() { return "echo"; }
    static _consteval auto description() { return "Repeats its arguments, separated by spaces, to the output"; }

    [[no_unique_address]] typename Config::basic_logger_type log; 

    template<typename ... Components>
    int main(int argc, char ** argv, std::tuple<Components...>&)
    {
        for (int i = 1; i < argc; ++i)
        {
            log.print(argv[i]);
            if (i + 1 < argc) log.print(" ");
        }
        log.println();
        return 0;
    };
};
template<typename Config>
struct HelloWorld
{
    static _consteval auto name() { return "hello"; }
    static _consteval auto description() { return "Say's 'Hello world!' Useful for testing the CLI"; }

    [[no_unique_address]] typename Config::basic_logger_type log; 

    template<typename ... Components>
    int main(int argc, char ** argv, std::tuple<Components...>&)
    {
        log.println("Hello world!");
        return 0;
    };
};
struct Command1 {
    static _consteval auto name() { return "test-command-1"; }
    static _consteval auto usage() { return "foo bar"; }
    static _consteval auto description() { return "Description 1"; }
};

struct Command2 {
    static _consteval auto name() { return "test-command-2"; }
    // no arguments, no usage text
    static _consteval auto description() { return "Description 2"; }
};

struct Component1 {
    static _consteval auto name() { return "Test Component 1"; }
};

struct Component2 {
    static _consteval auto name() { return "Test Component 2"; }
};

struct Config
{
    using basic_logger_type = sygaldry::bindings::basic_logger::TestLogger;
};

TEST_CASE("CLI", "[bindings][cli]")
{
    auto components = std::make_shared<std::tuple<Component1, Component2>>();
    auto cli = make_cli<Config, Echo, HelloWorld>(components);

    SECTION("Hello world")
    {
        REQUIRE(cli_process_input(cli, "hello\n") == "Hello world!\n> ");
    }

    SECTION("Echo")
    {
        REQUIRE(cli_process_input(cli, "echo foo bar baz\n") == "foo bar baz\n> ");
    }
}
TEST_CASE("List command outputs", "[cli][commands][list]")
{
    int argc = 1;
    char * arg = (char *)"list";
    char ** argv = &arg;

    List<Config> command;

    auto components = std::make_tuple(Component1{}, Component2{});
    auto retcode = command.main(argc, argv, components);

    REQUIRE(command.log.put.ss.str() == string("test-component-1\ntest-component-2\n"));
    REQUIRE(retcode == 0);
}
TEST_CASE("Help command", "[cli][commands][help]")
{
    Help<Config> command;

    auto retcode = command.main(Command1{}, Command2{});

    REQUIRE(command.log.put.ss.str() == string("test-command-1 foo bar\n    Description 1\ntest-command-2\n    Description 2\nhelp\n    Describe the available commands and their usage\n"));
    REQUIRE(retcode == 0);
}
TEST_CASE("Descibe", "[bindings][cli][commands][describe]")
{
    int argc = 3;
    char * arg0 = (char *)"describe";
    char * arg1 = (char *)"test-component-1";
    char * arg2 = (char *)"slider-out";
    char * wild = (char *)"*";
    char * argv[] = {arg0, arg1, arg2};
    auto components = std::make_tuple(sygaldry::components::TestComponent{});

    argc = 2;
    SECTION("describe device")
    {
        Describe<Config> command;
        auto retcode = command.main(argc, argv, components);
        REQUIRE(command.log.put.ss.str() == string("name: \"Test Component 1\"\n  inputs:\n    button-in\n    toggle-in\n    slider-in\n    bang-in\n  outputs:\n    button-out\n    toggle-out\n    slider-out\n    bang-out\n"));
        REQUIRE(retcode == 0);
    }

    argc = 2;
    argv[1] = wild;
    SECTION("describe all devices")
    {
        Describe<Config> command;
        auto retcode = command.main(argc, argv, components);
        REQUIRE(command.log.put.ss.str() == string("name: \"slider out\"\ntype: persistent float value\nrange: 0.0 to 1.0\ninit: 0.0\n"));
        REQUIRE(retcode == 0);
    }

    argc = 3;
    SECTION("describe endpoint")
    {
        Describe<Config> command;
        auto retcode = command.main(argc, argv, components);
        REQUIRE(command.log.put.ss.str() == string("name: \"slider out\"\ntype: persistent float value\nrange: 0.0 to 1.0\ninit: 0.0\n"));
        REQUIRE(retcode == 0);
    }

    argv[1] = arg1;
    argv[2] = wild;
    SECTION("describe all endpoints")
    {
        Describe<Config> command;
        auto retcode = command.main(argc, argv, components);
        REQUIRE(command.log.put.ss.str() == string("name: \"slider out\"\ntype: persistent float value\nrange: 0.0 to 1.0\ninit: 0.0\n"));
        REQUIRE(retcode == 0);
    }

    argv[1] = wild;
    argv[2] = wild;
    SECTION("describe everything")
    {
        Describe<Config> command;
        auto retcode = command.main(argc, argv, components);
        REQUIRE(command.log.put.ss.str() == string("name: \"slider out\"\ntype: persistent float value\nrange: 0.0 to 1.0\ninit: 0.0\n"));
        REQUIRE(retcode == 0);
    }
}
