
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <type_traits>
#include <boost/pfr.hpp>
#include "helpers.hpp"
#include "concepts.hpp"

using namespace sygaldry::endpoints;
using namespace sygaldry::concepts;
using std::string_view;
using std::is_aggregate_v;

template<string_literal str>
_consteval auto name() {return string_view{str.value};}

TEST_CASE("String literal", "[endpoints][string_literal]")
{
    REQUIRE(string_view(string_literal{"Hello world"}.value) == "Hello world");
    REQUIRE(name<"test">() == "test");
}
struct struct_with_name : named<"foo"> {};
TEST_CASE("Named", "[endpoints][bases][named]")
{
    REQUIRE(string_view(struct_with_name::name()) == string_view("foo"));
    static_assert(is_aggregate_v<struct_with_name>);
}
struct base_struct_with_name {static _consteval auto name() {return "yup";}};
TEST_CASE("get_name", "[components][endpoints][concepts][get_name]")
{
    static_assert(Named<base_struct_with_name>);
    static_assert(Named<struct_with_name>);
    struct_with_name foo{};
    base_struct_with_name yup{};
    REQUIRE(string_view(get_name(foo)) == string_view("foo"));
    REQUIRE(string_view(get_name<struct_with_name>()) == string_view("foo"));
    REQUIRE(string_view(get_name(yup)) == string_view("yup"));
    REQUIRE(string_view(get_name<base_struct_with_name>()) == string_view("yup"));
}
struct struct_with_range : with<range{0, 127}> {};
struct struct_with_init : with<range{0.0f, 100.0f, 42.0f}> {};

TEST_CASE("Range", "[endpoints][bases][range]")
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
TEST_CASE("Ranged", "[components][endpoints][concepts][ranged]")
{
    static_assert(Ranged<struct_with_range>);
    struct_with_range foo{};
    REQUIRE(get_range(foo).min == 0);
    REQUIRE(get_range(foo).max == 127);
    REQUIRE(get_range(foo).init == 0);
    REQUIRE(get_range<struct_with_range>().min == 0);
    REQUIRE(get_range<struct_with_range>().max == 127);
    REQUIRE(get_range<struct_with_range>().init == 0);
}
struct persistent_struct : persistent<int> {using persistent<int>::operator=;};
TEST_CASE("Persistent Value", "[endpoints][helpers][persistent]")
{
    auto s = persistent_struct{42};
    REQUIRE(s == 42);
    s = 88;
    REQUIRE(s == 88);
    static_assert(is_aggregate_v<persistent_struct>);
    static_assert(PersistentValue<persistent_struct>);
}
struct occasional_struct : occasional<int> { using occasional<int>::operator=; };
TEST_CASE("Occasional Value", "[endpoints][helpers][occasional]")
{
    auto s = occasional_struct{42};
    REQUIRE(bool(s) == true);
    REQUIRE(*s == 42);
    *s = 88;
    REQUIRE(*s == 88);
    s = decltype(s){};
    REQUIRE(bool(s) == false);
    static_assert(is_aggregate_v<occasional_struct>);
    static_assert(OccasionalValue<occasional_struct>);
}
TEST_CASE("Basic Endpoints", "[endpoints][basic]")
{
    static_assert(is_aggregate_v<button<"foo">>);
    static_assert(is_aggregate_v<toggle<"bar">>);
    static_assert(is_aggregate_v<slider<"baz">>);
}
TEST_CASE("Bang", "[endpoints][bang]")
{
    auto b = bng<"foo">{};
    REQUIRE(bool(b) == false);
    b();
    REQUIRE(bool(b) == true);
    b.reset();
    REQUIRE(bool(b) == false);
    b = true;
    REQUIRE(bool(b) == true);
    b = {};
    REQUIRE(bool(b) == false);
    static_assert(is_aggregate_v<decltype(b)>);
    static_assert(Bang<decltype(b)>);
    static_assert(sizeof(decltype(b)) == 1);
}
