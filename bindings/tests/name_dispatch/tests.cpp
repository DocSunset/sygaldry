#include <string_view>
#include <tuple>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "components/testcomponent.hpp"
#include "components/endpoints/helpers.hpp"
#include "bindings/basic_logger/test_logger.hpp"
#include "bindings/name_dispatch.hpp"

using std::string_view;

using namespace sygaldry::bindings;
using namespace sygaldry::endpoints;

struct Config
{
    using basic_logger_type = basic_logger::TestLogger;
};

struct Named1 { static _consteval auto name() {return "name1";} int value = 0;};
struct Named2 { static _consteval auto name() {return "name2";} float value = 0;};
struct Named3 : named<"name3">, persistent<int> {};
struct Named4 : named<"name4">, persistent<float> {};

struct PseudoComponent1
{
    static _consteval auto name() {return "pseudo-component";}
    struct {
        Named1 in1;
        Named3 in2;
    } inputs;
    struct {
        Named2 out1;
        Named4 out2;
    } outputs;
};

TEST_CASE("Dispatch over tuple of named", "[bindings][dispatch]")
{
    std::tuple tup{Named1{}, Named2{}, Named3{}, Named4{}};
    auto ret = dispatch("name1", tup, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name1"));
}

TEST_CASE("Dispatch can mutate entities in tuple", "[bindings][dispatch]")
{
    std::tuple tup{Named1{}, Named2{}, Named3{}, Named4{}};
    REQUIRE(std::get<Named1>(tup).value == 0);
    auto ret = dispatch("name1", tup, 0, [](auto& n) {n.value = 42; return (int)n.value;});
    REQUIRE(ret == 42);
    REQUIRE(std::get<Named1>(tup).value == 42);
}

//TEST_CASE("Dispatch over struct of endpoints", "[bindings][dispatch]")
//{
//    PseudoComponent1 x{};
//    auto ret = dispatch("name1", x.inputs, "fail", [](auto& n) {return n.name();});
//    REQUIRE(string_view(ret) == string_view("name1"));
//}
//
//TEST_CASE("Dispatch can mutate entities in struct of endpoints", "[bindings][dispatch]")
//{
//    PseudoComponent1 x{};
//    REQUIRE(x.inputs.in1.value == 0);
//    auto ret = dispatch("name1", x.in1, 0, [](auto& n) {n.value = 42; return (int)n.value;});
//    REQUIRE(ret == 42);
//    REQUIRE(x.inputs.in1.value == 42);
//}

