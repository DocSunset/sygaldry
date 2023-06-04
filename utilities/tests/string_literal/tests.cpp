#include <string_view>
#include <catch2/catch_test_macros.hpp>
#include "utilities/string_literal.hpp"

using namespace sygaldry::utilities;
using std::string_view;

template<string_literal str>
_consteval auto name() {return string_view{str.value};}

TEST_CASE("String literal", "[utilities][string_literal]")
{
    REQUIRE(string_view(string_literal{"Hello world"}.value) == "Hello world");
    REQUIRE(name<"test">() == "test");
}
