#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include <type_traits>
#include <boost/pfr.hpp>
#include "components/metadata.hpp"
#include "concepts/metadata.hpp"

using namespace sygaldry::concepts;
using std::string_view;

struct struct_with_name : name_<"foo"> {};
struct base_struct_with_name {static _consteval auto name() {return "yup";}};
TEST_CASE("get_name", "[components][concepts][get_name]")
{
    static_assert(has_name<base_struct_with_name>);
    static_assert(has_name<struct_with_name>);
    struct_with_name foo{};
    base_struct_with_name yup{};
    SECTION("T")
    {
        REQUIRE(string_view(get_name(foo)) == string_view("foo"));
        REQUIRE(string_view(get_name<struct_with_name>()) == string_view("foo"));
        REQUIRE(string_view(get_name(yup)) == string_view("yup"));
        REQUIRE(string_view(get_name<base_struct_with_name>()) == string_view("yup"));
    }
    SECTION("T&")
    {
        auto& bar = foo;
        auto& baz = yup;
        REQUIRE(string_view(get_name(bar)) == string_view("foo"));
        REQUIRE(string_view(get_name<struct_with_name&>()) == string_view("foo"));
        REQUIRE(string_view(get_name(baz)) == string_view("yup"));
        REQUIRE(string_view(get_name<base_struct_with_name&>()) == string_view("yup"));
    }
    SECTION("const T&")
    {
        const auto& bar = foo;
        const auto& baz = yup;
        REQUIRE(string_view(get_name(bar)) == string_view("foo"));
        REQUIRE(string_view(get_name<const struct_with_name&>()) == string_view("foo"));
        REQUIRE(string_view(get_name(baz)) == string_view("yup"));
        REQUIRE(string_view(get_name<const base_struct_with_name&>()) == string_view("yup"));
    }
}
