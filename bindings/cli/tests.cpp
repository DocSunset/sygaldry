#include <catch2/catch_test_macros.hpp>
#include "cli.hpp"
#include "bindings/basic_logger/basic_logger.hpp"
#include "bindings/basic_logger/test_putter.hpp"
#include <string>

using std::string;

using namespace bindings::cli::commands;
using namespace bindings::cli;

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

struct Device1 {
    static consteval auto name() { return "Test Device 1"; }
};

struct Device2 {
    static consteval auto name() { return "Test Device 2"; }
};

struct Config
{
    using basic_logger_type = bindings::basic_logger::BasicLogger<bindings::basic_logger::TestPutter>;
};

TEST_CASE("List command outputs", "[cli][commands][list]")
{
    int argc = 1;
    char * arg = (char *)"list";
    char ** argv = &arg;

    List<Config, Device1, Device2> command;

    auto devices = std::make_tuple(Device1{}, Device2{});
    auto retcode = command.main(argc, argv, devices);

    REQUIRE(command.log.put.ss.str() == string("test-device-1\ntest-device-2\n"));
    REQUIRE(retcode == 0);
}
TEST_CASE("Help command", "[cli][commands][help]")
{
    Help<Config, Command1, Command2> command;

    auto retcode = command.main();

    REQUIRE(command.log.put.ss.str() == string("test-command-1 foo bar\n    Description 1\ntest-command-2\n    Description 2\n"));
    REQUIRE(retcode == 0);
}
