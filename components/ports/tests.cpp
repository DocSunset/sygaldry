
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include "ports.hpp"

using namespace sygaldry::ports;
using std::string_view;

template<string_literal str>
consteval auto name() {return string_view{str.value};}

TEST_CASE("String literal", "[ports][string_literal]")
{
    REQUIRE(string_view(string_literal{"Hello world"}.value) == "Hello world");
    REQUIRE(name<"test">() == "test");
}
struct struct_with_name : named<"foo"> {};
TEST_CASE("Named", "[ports][bases][named]")
{
    REQUIRE(struct_with_name::name() == "foo");
}
struct struct_with_range : ranged<0, 127> {};

TEST_CASE("Ranged", "[port][bases][ranged]")
{
    REQUIRE(struct_with_range::range().min == 0);
    REQUIRE(struct_with_range::range().max == 127);
}
