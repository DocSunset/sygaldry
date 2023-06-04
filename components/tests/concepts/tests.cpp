
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <type_traits>
#include <boost/pfr.hpp>
#include "components/endpoints.hpp"
#include "components/concepts.hpp"

using namespace sygaldry::endpoints;
using namespace sygaldry::concepts;
using std::string_view;

struct struct_with_name : named<"foo"> {};
struct base_struct_with_name {static _consteval auto name() {return "yup";}};
TEST_CASE("get_name", "[components][endpoints][concepts][get_name]")
{
    static_assert(Named<base_struct_with_name>);
    static_assert(Named<struct_with_name>);
    struct_with_name foo{};
    // TODO: add checks passing cv-ref qualified foo
    base_struct_with_name yup{};
    REQUIRE(string_view(get_name(foo)) == string_view("foo"));
    REQUIRE(string_view(get_name<struct_with_name>()) == string_view("foo"));
    REQUIRE(string_view(get_name(yup)) == string_view("yup"));
    REQUIRE(string_view(get_name<base_struct_with_name>()) == string_view("yup"));
}
struct struct_with_range : with<range{0, 127}> {};
struct struct_with_init : with<range{0.0f, 100.0f, 42.0f}> {};
TEST_CASE("Ranged", "[components][endpoints][concepts][ranged]")
{
    static_assert(Ranged<struct_with_range>);
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
