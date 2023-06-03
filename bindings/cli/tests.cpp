#include <catch2/catch_test_macros.hpp>
#include "cli.hpp"
#include "bindings/basic_logger/test_logger.hpp"
#include <string>

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
    static consteval auto name() { return "echo"; }
    static consteval auto description() { return "Repeats its arguments, separated by spaces, to the output"; }

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
    static consteval auto name() { return "hello"; }
    static consteval auto description() { return "Say's 'Hello world!' Useful for testing the CLI"; }

    [[no_unique_address]] typename Config::basic_logger_type log; 

    template<typename ... Components>
    int main(int argc, char ** argv, std::tuple<Components...>&)
    {
        log.println("Hello world!");
        return 0;
    };
};
struct Command1 {
    static consteval auto name() { return "test-command-1"; }
    static consteval auto usage() { return "foo bar"; }
    static consteval auto description() { return "Description 1"; }
};

struct Command2 {
    static consteval auto name() { return "test-command-2"; }
    // no arguments, no usage text
    static consteval auto description() { return "Description 2"; }
};

struct Component1 {
    static consteval auto name() { return "Test Component 1"; }
};

struct Component2 {
    static consteval auto name() { return "Test Component 2"; }
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
