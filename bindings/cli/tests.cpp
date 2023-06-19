#include <string>
#include <memory>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "concepts/components.hpp"
#include "components/tests/testcomponent.hpp"
#include "bindings/basic_logger/test_logger.hpp"
#include "bindings/basic_reader/test_reader.hpp"
#include "cli.hpp"

using std::string;

using namespace sygaldry::bindings::clicommands;
using namespace sygaldry::bindings;
using namespace sygaldry;


void test_cli(auto& cli, auto& components, string input, string expected_output)
{
    cli.log.put.ss.str("");
    cli.reader.ss.str(input);
    cli(components);
    REQUIRE(cli.log.put.ss.str() == expected_output);
}

void test_command(auto&& command, auto&& components, int expected_retcode, string expected_output, auto ... args)
{
    int argc = 0;
    char * argv[sizeof...(args)];
    auto set_arg = [&](auto arg) {argv[argc++] = (char *)arg;};
    ( set_arg(args), ... );

    TestLogger logger{};
    logger.put.ss.str("");
    int retcode = command.main(argc, argv, logger, components);
    REQUIRE(retcode == expected_retcode);
    REQUIRE(logger.put.ss.str() == string(expected_output));
};

struct Echo
{
    static _consteval auto name() { return "echo"; }
    static _consteval auto description() { return "Repeats its arguments, separated by spaces, to the output"; }

    int main(int argc, char ** argv, auto& log, auto&)
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
struct HelloWorld
{
    static _consteval auto name() { return "hello"; }
    static _consteval auto description() { return "Say's 'Hello world!' Useful for testing the CLI"; }

    int main(int argc, char ** argv, auto& log, auto&)
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
    static _consteval auto name() { return "Test Component A"; }
};

struct Component2 {
    static _consteval auto name() { return "Test Component B"; }
};

struct TestComponents
{
    Component1 cpt1;
    Component2 cpt2;
    sygaldry::components::TestComponent tc;
};

struct CliCommands
{
    Echo echo;
    HelloWorld hello;
};

TEST_CASE("CLI", "[bindings][cli]")
{
    auto components = TestComponents{};
    auto cli = CustomCli<TestReader, TestLogger, TestComponents, CliCommands>{};
    static_assert(Component<decltype(cli)>);

    SECTION("Hello world")
    {
        test_cli(cli, components, "hello\n", "Hello world!\n> ");
    }

    SECTION("Echo")
    {
        test_cli(cli, components, "echo foo bar baz\n", "foo bar baz\n> ");
    }
}
TEST_CASE("Help command", "[cli][commands][help]")
{
    Help command;

    TestLogger logger{};
    logger.put.ss.str("");
    auto commands = TestCommands{};
    auto retcode = command.main(logger, commands);

    REQUIRE(logger.put.ss.str() == string("test-command-1 foo bar\n    Description 1\ntest-command-2\n    Description 2\nhelp\n    Describe the available commands and their usage\n"));
    REQUIRE(retcode == 0);
}
TEST_CASE("List command outputs", "[cli][commands][list]")
{
    test_command(List{}, TestComponents{},
                 0, "test-component-a\ntest-component-b\ntest-component-1\n",
                 "list");
}
TEST_CASE("Descibe", "[bindings][cli][commands][describe]")
{
    auto components = TestComponents{};
    components.tc.inputs.button_in = 1;
    components.tc.inputs.bang_in();
    test_command(Describe{}, components, 0,
R"DESCRIBEDEVICE(component: test-component-1
  name: "Test Component 1"
  type:  component
  input:   button-in
    name: "button in"
    type:  occasional value
    range: 0 to 1 (init: 0)
    value: (1)
  input:   toggle-in
    name: "toggle in"
    type:  persistent value
    range: 0 to 1 (init: 0)
    value: 0
  input:   slider-in
    name: "slider in"
    type:  persistent value
    range: 0 to 1 (init: 0)
    value: 0
  input:   bang-in
    name: "bang in"
    type:  bang
    value: (bang!)
  output:  button-out
    name: "button out"
    type:  occasional value
    range: 0 to 1 (init: 0)
    value: ()
  output:  toggle-out
    name: "toggle out"
    type:  persistent value
    range: 0 to 1 (init: 0)
    value: 0
  output:  slider-out
    name: "slider out"
    type:  persistent value
    range: 0 to 1 (init: 0)
    value: 0
  output:  bang-out
    name: "bang out"
    type:  bang
    value: ()
)DESCRIBEDEVICE", "describe", "test-component-1");

    test_command(Describe{}, TestComponents{}, 0,
R"DESCRIBEENDPOINT(endpoint: slider-out
  name: "slider out"
  type:  persistent value
  range: 0 to 1 (init: 0)
  value: 0
)DESCRIBEENDPOINT", "describe", "test-component-1", "slider-out");
}
TEST_CASE("Set", "[bindings][cli][commands][set]")
{
    auto components = TestComponents{};
    SECTION("set slider")
    {
        test_command(Set{}, components, 0, "", "set", "test-component-1", "slider-in", "0.31459");
        REQUIRE(components.tc.inputs.slider_in.value == 0.31459f);
    }

    SECTION("set toggle")
    {
        REQUIRE(components.tc.inputs.toggle_in.value == 0);
        test_command(Set{}, components, 0, "", "set", "test-component-1", "toggle-in", "1");
        REQUIRE(components.tc.inputs.toggle_in.value == 1);
    }

    SECTION("set button")
    {
        REQUIRE(not components.tc.inputs.button_in);
        test_command(Set{}, components, 0, "", "set", "test-component-1", "button-in", "1");
        REQUIRE(components.tc.inputs.button_in);
        REQUIRE(components.tc.inputs.button_in.value() == 1);
    }

    SECTION("set bang")
    {
        test_command(Set{}, components, 0, "", "set", "test-component-1", "bang-in");
        REQUIRE(components.tc.inputs.bang_in.value == true);
    }
}
TEST_CASE("Trigger", "[bindings][cli][commands][trigger]")
{
    auto components = TestComponents{};
    // TODO: test this properly
}
