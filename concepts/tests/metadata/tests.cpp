/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include <type_traits>
#include <boost/pfr.hpp>
#include "components/metadata.hpp"
#include "concepts/metadata.hpp"

using namespace sygaldry;
using std::string_view;

struct struct_with_name : name_<"foo"> {};
struct base_struct_with_name {static _consteval auto name() {return "yup";}};
TEST_CASE("name_of", "[components][concepts][name_of]")
{
    static_assert(has_name<base_struct_with_name>);
    static_assert(has_name<struct_with_name>);
    struct_with_name foo{};
    base_struct_with_name yup{};
    SECTION("T")
    {
        REQUIRE(string_view(name_of(foo)) == string_view("foo"));
        REQUIRE(string_view(name_of<struct_with_name>()) == string_view("foo"));
        REQUIRE(string_view(name_of(yup)) == string_view("yup"));
        REQUIRE(string_view(name_of<base_struct_with_name>()) == string_view("yup"));
    }
    SECTION("T&")
    {
        auto& bar = foo;
        auto& baz = yup;
        REQUIRE(string_view(name_of(bar)) == string_view("foo"));
        REQUIRE(string_view(name_of<struct_with_name&>()) == string_view("foo"));
        REQUIRE(string_view(name_of(baz)) == string_view("yup"));
        REQUIRE(string_view(name_of<base_struct_with_name&>()) == string_view("yup"));
    }
    SECTION("const T&")
    {
        const auto& bar = foo;
        const auto& baz = yup;
        REQUIRE(string_view(name_of(bar)) == string_view("foo"));
        REQUIRE(string_view(name_of<const struct_with_name&>()) == string_view("foo"));
        REQUIRE(string_view(name_of(baz)) == string_view("yup"));
        REQUIRE(string_view(name_of<const base_struct_with_name&>()) == string_view("yup"));
    }
}
