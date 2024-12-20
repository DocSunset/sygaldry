/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include "sygac-metadata.hpp"

using namespace sygaldry;
using std::string_view;

struct base_struct_with_name {static constexpr auto name() {return "yup";}};
TEST_CASE("sygaldry name_of", "[components][concepts][name_of]")
{
    static_assert(has_name<base_struct_with_name>);
    base_struct_with_name yup{};
    SECTION("T")
    {
        REQUIRE(string_view(name_of(yup)) == string_view("yup"));
        REQUIRE(string_view(name_of<base_struct_with_name>()) == string_view("yup"));
    }
    SECTION("T&")
    {
        auto& baz = yup;
        REQUIRE(string_view(name_of(baz)) == string_view("yup"));
        REQUIRE(string_view(name_of<base_struct_with_name&>()) == string_view("yup"));
    }
    SECTION("const T&")
    {
        const auto& baz = yup;
        REQUIRE(string_view(name_of(baz)) == string_view("yup"));
        REQUIRE(string_view(name_of<const base_struct_with_name&>()) == string_view("yup"));
    }
}
