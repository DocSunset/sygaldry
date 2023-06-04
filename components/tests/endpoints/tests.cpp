
#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <type_traits>
#include <optional>
#include <boost/pfr.hpp>
#include "components/endpoints.hpp"
#include "components/concepts.hpp"

using namespace sygaldry::endpoints;
using namespace sygaldry::concepts;
using std::string_view;

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
    }
    SECTION("With init")
    {
        REQUIRE(struct_with_init::range().init == 42.0f);
    }
}
struct persistent_struct : persistent<int> {using persistent<int>::operator=;};
TEST_CASE("Persistent Value", "[endpoints][helpers][persistent]")
{
    auto s = persistent_struct{42};
    REQUIRE(s == 42);
    s = 88;
    REQUIRE(s == 88);
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
    static_assert(OccasionalValue<occasional_struct>);
}
TEST_CASE("Basic Endpoints", "[endpoints][basic]")
{
    static_assert(OccasionalValue<button<"foo">>);
    static_assert(PersistentValue<toggle<"baz">>);
    static_assert(PersistentValue<slider<"baz">>);
    static_assert(Ranged<button<"foo">>);
    static_assert(Ranged<toggle<"bar">>);
    static_assert(Ranged<slider<"baz">>);
    static_assert(Named<button<"foo">>);
    static_assert(Named<toggle<"bar">>);
    static_assert(Named<slider<"baz">>);
    static_assert(OccasionalValue<button<"foo">&>);
    static_assert(PersistentValue<toggle<"bar">&>);
    static_assert(PersistentValue<slider<"baz">&>);
    static_assert(Ranged<button<"foo">&>);
    static_assert(Ranged<toggle<"bar">&>);
    static_assert(Ranged<slider<"baz">&>);
    static_assert(Named<button<"foo">&>);
    static_assert(Named<toggle<"bar">&>);
    static_assert(Named<slider<"baz">&>);
    static_assert(OccasionalValue<const button<"foo">&>);
    static_assert(PersistentValue<const toggle<"bar">&>);
    static_assert(PersistentValue<const slider<"baz">&>);
    static_assert(Ranged<const button<"foo">&>);
    static_assert(Ranged<const toggle<"bar">&>);
    static_assert(Ranged<const slider<"baz">&>);
    static_assert(Named<const button<"foo">&>);
    static_assert(Named<const toggle<"bar">&>);
    static_assert(Named<const slider<"baz">&>);
    static_assert(sizeof(button<"foo">) == sizeof(std::optional<bool>));
    static_assert(sizeof(toggle<"baz">) == sizeof(bool));
    static_assert(sizeof(slider<"baz">) == sizeof(float));
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
    static_assert(Bang<decltype(b)>);
    static_assert(sizeof(decltype(b)) == sizeof(bool));
}
