#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include <type_traits>
#include <optional>
#include <boost/pfr.hpp>
#include "helpers/metadata.hpp"

using namespace sygaldry;

using std::string_view;

template<string_literal str>
_consteval auto name() {return string_view{str.value};}

TEST_CASE("String literal", "[endpoints][string_literal]")
{
    REQUIRE(string_view(string_literal{"Hello world"}.value) == "Hello world");
    REQUIRE(name<"test">() == "test");
}

struct struct_with_name : name_<"foo"> {};
TEST_CASE("name_", "[endpoints][bases][name_]")
{
    REQUIRE(string_view(struct_with_name::name()) == string_view("foo"));
}
