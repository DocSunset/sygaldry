
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <type_traits>
#include <boost/pfr.hpp>
#include "ports.hpp"
#include "concepts.hpp"

using namespace sygaldry::concepts;
using namespace sygaldry::ports;
using std::string_view;
using std::is_aggregate_v;

template<string_literal str>
_consteval auto name() {return string_view{str.value};}

TEST_CASE("String literal", "[ports][string_literal]")
{
    REQUIRE(string_view(string_literal{"Hello world"}.value) == "Hello world");
    REQUIRE(name<"test">() == "test");
}
struct struct_with_name : named<"foo"> {};
TEST_CASE("Named", "[ports][bases][named]")
{
    REQUIRE(struct_with_name::name() == "foo");
    static_assert(is_aggregate_v<struct_with_name>);
}
struct base_struct_with_name {static _consteval auto name() {return "yup";}};
TEST_CASE("Named", "[components][ports][concepts][named]")
{
    static_assert(Named<base_struct_with_name>);
    //static_assert(Named<struct_with_name>);
    struct_with_name foo{};
    base_struct_with_name yup{};
    REQUIRE(get_name(foo) == "foo");
    REQUIRE(get_name<struct_with_name>() == "foo");
    REQUIRE(get_name(yup) == "yup");
    REQUIRE(get_name<base_struct_with_name>() == "yup");
}
struct struct_with_range : with<range{0, 127}> {};
struct struct_with_init : with<range{0.0f, 100.0f, 42.0f}> {};

TEST_CASE("Range", "[port][bases][range]")
{
    SECTION("With range")
    {
        REQUIRE(struct_with_range::range().min == 0);
        REQUIRE(struct_with_range::range().max == 127);
        REQUIRE(struct_with_range::range().init == 0);
        static_assert(is_aggregate_v<struct_with_range>);
    }
    SECTION("With init")
    {
        REQUIRE(struct_with_init::range().init == 42.0f);
        static_assert(is_aggregate_v<struct_with_init>);
    }
}
struct persistent_struct : persistent<int> {using persistent<int>::operator=;};
TEST_CASE("Persistent Value", "[ports][bases][persistent]")
{
    auto s = persistent_struct{42};
    REQUIRE(s == 42);
    s = 88;
    REQUIRE(s == 88);
    static_assert(is_aggregate_v<persistent_struct>);
}
struct occasional_struct : occasional<int> { using occasional<int>::operator=; };
TEST_CASE("Occasional Value", "[ports][bases][occasional]")
{
    auto s = occasional_struct{42};
    s = 42;
    REQUIRE(s);
    REQUIRE(s == 42);
    s = 88;
    REQUIRE(s == 88);
    auto null = occasional_struct{};
    REQUIRE(!null);
    static_assert(is_aggregate_v<occasional_struct>);
}
TEST_CASE("Basic Ports", "[ports][basic]")
{
    static_assert(is_aggregate_v<button<"foo">>);
    static_assert(is_aggregate_v<toggle<"bar">>);
    static_assert(is_aggregate_v<slider<"baz">>);
}
TEST_CASE("Bang", "[ports][bang]")
{
    static_assert(is_aggregate_v<bng<"foo">>);
    auto b = bng<"foo">{};
    REQUIRE(!b);
    b();
    REQUIRE(b);
    b = {};
    REQUIRE(!b);
}
