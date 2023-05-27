#include <catch2/catch_test_macros.hpp>
#include "cli.hpp"
#include <string>

using std::string;

using namespace bindings::cli::commands;

struct Device1 {
    static consteval auto name() { return "Test Device 1"; }
};

struct Device2 {
    static consteval auto name() { return "Test Device 2"; }
};

TEST_CASE("List command outputs", "[cli][commands][list]")
{
    int argc = 1;
    char * arg = (char *)"list";
    char ** argv = &arg;

    List<Device1, Device2> command;

    auto devices = std::make_tuple(Device1{}, Device2{});
    command.main(argc, argv, devices);

    REQUIRE(detail::list_output_length_v<Device1, Device2> == 29);
    REQUIRE(string(command.outputs.stdout.value) == string("test-device-1\ntest-device-2\n"));
    REQUIRE(command.outputs.retcode.value == 0);
}
