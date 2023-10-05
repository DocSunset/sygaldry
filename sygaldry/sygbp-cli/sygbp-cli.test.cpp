/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <string>
#include <memory>
#include <catch2/catch_test_macros.hpp>
#include "sygah-consteval.hpp"
#include "sygac-components.hpp"
#include "sygbp-test_component.hpp"
#include "sygup-test_logger.hpp"
#include "sygbp-test_reader.hpp"
#include "sygbp-cli.hpp"

using std::string;

using namespace sygaldry::sygbp;
using namespace sygaldry::sygbp;
using namespace sygaldry;


void test_cli(auto& cli, auto& components, string input, string expected_output)
{
    cli.log.put.ss.str("");
    cli.reader.ss.str(input);
    cli.external_sources(components);
    REQUIRE(cli.log.put.ss.str() == expected_output);
}

void test_command(auto&& command, auto&& components, int expected_retcode, string expected_output, auto ... args)
{
    int argc = 0;
    char * argv[sizeof...(args)];
    auto set_arg = [&](auto arg) {argv[argc++] = (char *)arg;};
    ( set_arg(args), ... );

    sygup::TestLogger logger{};
    logger.put.ss.str("");
    int retcode = command.main(argc, argv, logger, components);
    REQUIRE(retcode == expected_retcode);
    REQUIRE(logger.put.ss.str() == string(expected_output));
};

struct Echo
{
    static _consteval auto name() { return "/echo"; }
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
    static _consteval auto name() { return "/hello"; }
    static _consteval auto description() { return "Say's 'Hello world!' Useful for testing the CLI"; }

    int main(int argc, char ** argv, auto& log, auto&)
    {
        log.println("Hello world!");
        return 0;
    };
};
struct Command1 {
    static _consteval auto name() { return "/test-command-1"; }
    static _consteval auto usage() { return "foo bar"; }
    static _consteval auto description() { return "Description 1"; }
};

struct Command2 {
    static _consteval auto name() { return "/test-command-2"; }
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
    void main() {}
};

struct Component2 {
    static _consteval auto name() { return "Test Component B"; }
    void main() {}
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

TEST_CASE("sygaldry CLI", "[bindings][cli]")
{
    auto components = TestComponents{};
    auto cli = CustomCli<TestReader, sygup::TestLogger, TestComponents, CliCommands>{};
    static_assert(Component<decltype(cli)>);

    SECTION("Hello world")
    {
        test_cli(cli, components, "/hello\n", "Hello world!\n> ");
    }

    SECTION("Echo")
    {
        test_cli(cli, components, "/echo foo bar baz\n", "foo bar baz\n> ");
    }
}
TEST_CASE("sygaldry Help command", "[cli][commands][help]")
{
    Help command;

    sygup::TestLogger logger{};
    logger.put.ss.str("");
    auto commands = TestCommands{};
    auto retcode = command.main(logger, commands);

    REQUIRE(logger.put.ss.str() == string("/test-command-1 foo bar\n    Description 1\n/test-command-2\n    Description 2\n/help\n    Describe the available commands and their usage\n"));
    REQUIRE(retcode == 0);
}
TEST_CASE("sygaldry List command outputs", "[cli][commands][list]")
{
    test_command(List{}, TestComponents{},
                 0, "/Test_Component_A\n/Test_Component_B\n/Test_Component_1\n",
                 "list");
}
TEST_CASE("sygaldry Descibe", "[bindings][cli][commands][describe]")
{
    auto components = TestComponents{};
    components.tc.inputs.button_in = 1;
    components.tc.inputs.bang_in();
    test_command(Describe{}, components, 0,
R"DESCRIBEDEVICE(entity: /Test_Component_1
  name: "Test Component 1"
  type:  component
  input:   /Test_Component_1/button_in
    name: "button in"
    type:  occasional int
    range: 0 to 1 (init: 0)
    value: (! 1 !)
  input:   /Test_Component_1/toggle_in
    name: "toggle in"
    type:  persistent int
    range: 0 to 1 (init: 0)
    value: 0
  input:   /Test_Component_1/slider_in
    name: "slider in"
    type:  persistent float
    range: 0 to 1 (init: 0)
    value: 0
  input:   /Test_Component_1/bang_in
    name: "bang in"
    type:  bang
    value: (! bang !)
  input:   /Test_Component_1/text_in
    name: "text in"
    type:  persistent text
    value: ""
  input:   /Test_Component_1/text_message_in
    name: "text message in"
    type:  occasional text
    value: ()
  input:   /Test_Component_1/array_in
    name: "array in"
    type:  array of float
    range: 0 to 1 (init: 0)
    value: [0 0 0]
  output:  /Test_Component_1/button_out
    name: "button out"
    type:  occasional int
    range: 0 to 1 (init: 0)
    value: (0)
  output:  /Test_Component_1/toggle_out
    name: "toggle out"
    type:  persistent int
    range: 0 to 1 (init: 0)
    value: 0
  output:  /Test_Component_1/slider_out
    name: "slider out"
    type:  persistent float
    range: 0 to 1 (init: 0)
    value: 0
  output:  /Test_Component_1/bang_out
    name: "bang out"
    type:  bang
    value: ()
  output:  /Test_Component_1/text_out
    name: "text out"
    type:  persistent text
    value: ""
  output:  /Test_Component_1/text_message_out
    name: "text message out"
    type:  occasional text
    value: ()
  output:  /Test_Component_1/array_out
    name: "array out"
    type:  array of float
    range: 0 to 1 (init: 0)
    value: [0 0 0]
)DESCRIBEDEVICE", "describe", "/Test_Component_1");

    test_command(Describe{}, TestComponents{}, 0,
R"DESCRIBEENDPOINT(entity: /Test_Component_1/slider_out
  name: "slider out"
  type:  persistent float
  range: 0 to 1 (init: 0)
  value: 0
)DESCRIBEENDPOINT", "describe", "/Test_Component_1/slider_out");
    components.tc.inputs.text_in = "hello";

    CHECK(components.tc.inputs.text_in.value == string("hello"));
    test_command(Describe{}, components, 0,
R"DESCRIBEENDPOINT(entity: /Test_Component_1/text_in
  name: "text in"
  type:  persistent text
  value: "hello"
)DESCRIBEENDPOINT", "describe", "/Test_Component_1/text_in");
}
TEST_CASE("sygaldry Set", "[bindings][cli][commands][set]")
{
    auto components = TestComponents{};
    SECTION("set slider")
    {
        test_command(Set{}, components, 0, "", "/set", "/Test_Component_1/slider_in", "0.31459");
        REQUIRE(components.tc.inputs.slider_in.value == 0.31459f);
    }

    SECTION("set toggle")
    {
        REQUIRE(components.tc.inputs.toggle_in.value == 0);
        test_command(Set{}, components, 0, "", "/set", "/Test_Component_1/toggle_in", "1");
        REQUIRE(components.tc.inputs.toggle_in.value == 1);
    }

    SECTION("set button")
    {
        REQUIRE(not components.tc.inputs.button_in.updated);
        test_command(Set{}, components, 0, "", "/set", "/Test_Component_1/button_in", "1");
        REQUIRE(components.tc.inputs.button_in.updated);
        REQUIRE(components.tc.inputs.button_in.value() == 1);
    }

    SECTION("set bang")
    {
        test_command(Set{}, components, 0, "", "/set", "/Test_Component_1/bang_in");
        REQUIRE(components.tc.inputs.bang_in.value == true);
    }

    SECTION("set string")
    {
        test_command(Set{}, components, 0, "", "/set", "/Test_Component_1/text_in", "helloworld");
        REQUIRE(components.tc.inputs.text_in.value == string("helloworld"));
    }

    SECTION("set array")
    {
        test_command(Set{}, components, 0, "", "/set", "/Test_Component_1/array_in", "1", "2", "3");
        REQUIRE(components.tc.inputs.array_in.value == std::array<float, 3>{1,2,3});
    }
}
