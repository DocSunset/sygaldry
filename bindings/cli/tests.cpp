#include <string>
#include <memory>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "components/tests/testcomponent.hpp"
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

    template<typename Components>
    int main(int argc, char ** argv, Components&)
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

    template<typename Components>
    int main(int argc, char ** argv, Components&)
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

struct TestCommands
{
    Command1 cmd1;
    Command2 cmd2;
};

struct Component1 {
    static _consteval auto name() { return "Test Component 1"; }
};

struct Component2 {
    static _consteval auto name() { return "Test Component 2"; }
};

struct TestComponents
{
    Component1 cpt1;
    Component2 cpt2;
};

struct Config
{
    using basic_logger_type = sygaldry::bindings::basic_logger::TestLogger;
};

template<typename Config>
struct CliCommands
{
    Echo<Config> echo;
    HelloWorld<Config> hello;
};

TEST_CASE("CLI", "[bindings][cli]")
{
    auto cpts = std::make_shared<TestComponents>();
    auto cli = make_cli<Config, CliCommands>(cpts);

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

    auto components = TestComponents{};
    command.log.put.ss.str("");
    auto retcode = command.main(argc, argv, components);

    REQUIRE(command.log.put.ss.str() == string("test-component-1\ntest-component-2\n"));
    REQUIRE(retcode == 0);
}
TEST_CASE("Help command", "[cli][commands][help]")
{
    Help<Config> command;

    command.log.put.ss.str("");
    auto commands = TestCommands{};
    auto retcode = command.main(commands);

    REQUIRE(command.log.put.ss.str() == string("test-command-1 foo bar\n    Description 1\ntest-command-2\n    Description 2\nhelp\n    Describe the available commands and their usage\n"));
    REQUIRE(retcode == 0);
}
struct DescribeComponents
{
    sygaldry::components::TestComponent tc;
};
TEST_CASE("Descibe", "[bindings][cli][commands][describe]")
{
    int argc = 3;
    char * arg0 = (char *)"describe";
    char * arg1 = (char *)"test-component-1";
    char * arg2 = (char *)"slider-out";
    char * argv[] = {arg0, arg1, arg2};
    auto components = DescribeComponents{};


    argc = 2;
    SECTION("describe device")
    {
        Describe<Config> command;
        command.log.put.ss.str("");
        auto retcode = command.main(argc, argv, components);
        REQUIRE(command.log.put.ss.str() == string(
R"DESCRIBEDEVICE(component: test-component-1
  name: "Test Component 1"
  type:  component
  input:   button-in
    name: "button in"
    type:  occasional value
    range: false to true (init: false)
  input:   toggle-in
    name: "toggle in"
    type:  persistent value
    range: false to true (init: false)
  input:   slider-in
    name: "slider in"
    type:  persistent value
    range: 0 to 1 (init: 0)
  input:   bang-in
    name: "bang in"
    type:  persistent value
  output:  button-out
    name: "button out"
    type:  occasional value
    range: false to true (init: false)
  output:  toggle-out
    name: "toggle out"
    type:  persistent value
    range: false to true (init: false)
  output:  slider-out
    name: "slider out"
    type:  persistent value
    range: 0 to 1 (init: 0)
  output:  bang-out
    name: "bang out"
    type:  persistent value
)DESCRIBEDEVICE"));
        REQUIRE(retcode == 0);
        command.log.put.ss.str("");
    }

    argc = 3;
    SECTION("describe endpoint")
    {
        Describe<Config> command;
        command.log.put.ss.str("");
        auto retcode = command.main(argc, argv, components);
        REQUIRE(command.log.put.ss.str() == string(
R"DESCRIBEENDPOINT(endpoint: slider-out
  name: "slider out"
  type:  persistent value
  range: 0 to 1 (init: 0)
)DESCRIBEENDPOINT"));

        REQUIRE(retcode == 0);
        command.log.put.ss.str("");
    }
}
