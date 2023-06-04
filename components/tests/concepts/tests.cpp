
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <type_traits>
#include <boost/pfr.hpp>
#include "components/endpoints.hpp"
#include "components/concepts.hpp"

using namespace sygaldry::endpoints;
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
        REQUIRE(string_view(get_name<struct_with_name>()) == string_view("foo"));
        REQUIRE(string_view(get_name(baz)) == string_view("yup"));
        REQUIRE(string_view(get_name<base_struct_with_name>()) == string_view("yup"));
    }
}
struct struct_with_range : range_<0, 127> {};
struct struct_with_init : range_<0.0f, 100.0f, 42.0f> {};
TEST_CASE("has_range", "[components][concepts][has_range]")
{
    static_assert(has_range<struct_with_range>);
    static_assert(has_range<struct_with_init>);
    struct_with_range foo{};
    SECTION("T")
    {
        REQUIRE(get_range(foo).min == 0);
        REQUIRE(get_range(foo).max == 127);
        REQUIRE(get_range(foo).init == 0);
        REQUIRE(get_range<struct_with_range>().min == 0);
        REQUIRE(get_range<struct_with_range>().max == 127);
        REQUIRE(get_range<struct_with_range>().init == 0);
        auto r1 = get_range(foo);
        auto r2 = get_range<struct_with_range>();
    }
    SECTION("T&")
    {
        auto& bar = foo;
        REQUIRE(get_range(bar).min == 0);
        REQUIRE(get_range(bar).max == 127);
        REQUIRE(get_range(bar).init == 0);
        REQUIRE(get_range<struct_with_range&>().min == 0);
        REQUIRE(get_range<struct_with_range&>().max == 127);
        REQUIRE(get_range<struct_with_range&>().init == 0);
        auto r3 = get_range(bar);
        auto r4 = get_range<struct_with_range&>();
    }
    SECTION("constT&")
    {
        const auto& baz = foo;
        REQUIRE(get_range(baz).min == 0);
        REQUIRE(get_range(baz).max == 127);
        REQUIRE(get_range(baz).init == 0);
        REQUIRE(get_range<struct_with_range&>().min == 0);
        REQUIRE(get_range<struct_with_range&>().max == 127);
        REQUIRE(get_range<struct_with_range&>().init == 0);
        auto r5 = get_range(baz);
        auto r6 = get_range<const struct_with_range&>();
    }
}
