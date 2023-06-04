#include <string_view>
#include <tuple>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "components/endpoints.hpp"
#include "bindings/basic_logger/test_logger.hpp"
#include "bindings/name_dispatch.hpp"

using std::string_view;

using namespace sygaldry::bindings;
using namespace sygaldry::endpoints;

struct Config
{
    using basic_logger_type = basic_logger::TestLogger;
};

struct Named1 { static _consteval auto name() {return "name1";} int value;};
struct Named2 { static _consteval auto name() {return "a longer name";} float value;};
struct Named3 : named<"name3">, persistent<int> {};
struct Named4 : named<"name4">, persistent<float> {};

struct PseudoComponent1
{
    static _consteval auto name() {return "pseudo-component";}
    struct inputs_t {
        Named1 in1;
        Named3 in2;
    } inputs;
    struct outputs_t {
        Named2 out1;
        Named4 out2;
    } outputs;
};

TEST_CASE("Dispatch over tuple of named", "[bindings][dispatch]")
{
    std::tuple tup{Named1{}, Named2{}, Named3{}, Named4{}};
    auto ret = dispatch("name1", tup, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name1"));
    ret = dispatch("name3", tup, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name3"));
}

TEST_CASE("Dispatch can mutate entities in tuple", "[bindings][dispatch]")
{
    std::tuple tup{Named1{}, Named2{}, Named3{}, Named4{}};
    REQUIRE(std::get<Named1>(tup).value == 0);
    auto ret = dispatch("name1", tup, 0, [](auto& n) {n.value = 42; return (int)n.value;});
    REQUIRE(ret == 42);
    REQUIRE(std::get<Named1>(tup).value == 42);
    ret = dispatch("name3", tup, 0, [](auto& n) {n.value = 888; return (int)n.value;});
    REQUIRE(ret == 888);
    REQUIRE(std::get<Named3>(tup).value == 888);
}

TEST_CASE("Dispatch over struct of endpoints", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    auto ret = dispatch("name1", x.inputs, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name1"));
    ret = dispatch("name3", x.inputs, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name3"));
}

TEST_CASE("Dispatch can mutate entities in struct of endpoints", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    REQUIRE(x.inputs.in1.value == 0);
    auto ret = dispatch("name1", x.inputs, 0, [](auto& n) {n.value = 42; return (int)n.value;});
    REQUIRE(ret == 42);
    REQUIRE(x.inputs.in1.value == 42);
    ret = dispatch("name3", x.inputs, 0, [](auto& n) {n.value = 888; return (int)n.value;});
    REQUIRE(ret == 888);
    REQUIRE(x.inputs.in2.value == 888);
}

TEST_CASE("Dispatch over component", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    auto ret = dispatch("name1", x, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name1"));
    ret = dispatch("name3", x, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name3"));
}

TEST_CASE("Dispatch can mutate entities component", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    REQUIRE(x.inputs.in1.value == 0);
    auto ret = dispatch("name1", x, 0, [](auto& n) {n.value = 42; return (int)n.value;});
    REQUIRE(ret == 42);
    REQUIRE(x.inputs.in1.value == 42);
    ret = dispatch("name3", x, 0, [](auto& n) {n.value = 888; return (int)n.value;});
    REQUIRE(ret == 888);
    REQUIRE(x.inputs.in2.value == 888);
}

struct CustomMatcher
{
    template<typename stringish, typename NamedT>
    bool operator()(stringish name, const NamedT& candidate)
    {
        return std::string_view(NamedT::name()) == std::string_view("name4");
    }
};

TEST_CASE("Dispatch with custom matcher", "[bindings][dispatch]")
{
    PseudoComponent1 x{};
    auto ret = dispatch<CustomMatcher>("name1", x, "fail", [](auto& n) {return n.name();});
    REQUIRE(string_view(ret) == string_view("name4"));
}