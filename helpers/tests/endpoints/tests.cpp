/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include <string_view>
#include <optional>
#include "sygaldry-concepts-metadata.hpp"
#include "sygaldry-concepts-endpoints.hpp"
#include "helpers/endpoints.hpp"

using namespace sygaldry;
using std::string_view;

struct struct_with_range : range_<0, 127> {};
struct struct_with_init : range_<0.0f, 100.0f, 42.0f> {};

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
struct tag_foo {enum {foo};};
struct tag_bar {enum {bar};};
template<typename ... Tags>
struct tag_helper_test : tagged_<Tags...> {};
tag_helper_test<> t1; // make sure this compiles
tag_helper_test<tag_foo> t2;
static_assert(t2.foo == tag_foo::foo);
tag_helper_test<tag_foo, tag_bar> t3;
static_assert(t3.foo == tag_foo::foo);
static_assert(t3.bar == tag_bar::bar);
TEST_CASE("Basic Endpoints", "[endpoints][basic]")
{
    static_assert(OccasionalValue<button<"foo">>);
    static_assert(PersistentValue<toggle<"baz">>);
    static_assert(PersistentValue<slider<"baz">>);
    static_assert(has_range<button<"foo">>);
    static_assert(has_range<toggle<"bar">>);
    static_assert(has_range<slider<"baz">>);
    static_assert(has_name<button<"foo">>);
    static_assert(has_name<toggle<"bar">>);
    static_assert(has_name<slider<"baz">>);
    static_assert(OccasionalValue<button<"foo">&>);
    static_assert(PersistentValue<toggle<"bar">&>);
    static_assert(PersistentValue<slider<"baz">&>);
    static_assert(has_range<button<"foo">&>);
    static_assert(has_range<toggle<"bar">&>);
    static_assert(has_range<slider<"baz">&>);
    static_assert(has_name<button<"foo">&>);
    static_assert(has_name<toggle<"bar">&>);
    static_assert(has_name<slider<"baz">&>);
    static_assert(OccasionalValue<const button<"foo">&>);
    static_assert(PersistentValue<const toggle<"bar">&>);
    static_assert(PersistentValue<const slider<"baz">&>);
    static_assert(has_range<const button<"foo">&>);
    static_assert(has_range<const toggle<"bar">&>);
    static_assert(has_range<const slider<"baz">&>);
    static_assert(has_name<const button<"foo">&>);
    static_assert(has_name<const toggle<"bar">&>);
    static_assert(has_name<const slider<"baz">&>);
    static_assert(sizeof(button<"foo">) <= sizeof(std::optional<bool>));
    static_assert(sizeof(toggle<"baz">) == sizeof(bool));
    static_assert(sizeof(slider<"baz">) == sizeof(float));

    auto s1 = slider<"baz">{};
    auto s2 = slider<"baz">{0.5f};
    s2 = 0.0f;
}
TEST_CASE("Bang", "[endpoints][bang]")
{
    auto b = bng<"foo">{};
    REQUIRE(bool(b) == false);
    REQUIRE(value_of(b) == false);
    b();
    REQUIRE(bool(b) == true);
    REQUIRE(value_of(b) == true);
    b.reset();
    REQUIRE(bool(b) == false);
    b = true;
    REQUIRE(bool(b) == true);
    b = {};
    REQUIRE(bool(b) == false);
    static_assert(Bang<decltype(b)>);
    static_assert(sizeof(decltype(b)) == sizeof(bool));
}
