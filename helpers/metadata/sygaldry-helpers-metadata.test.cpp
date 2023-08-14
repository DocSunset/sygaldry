/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include "sygaldry-helpers-metadata.hpp"

using namespace sygaldry;

using std::string_view;

template<string_literal str>
_consteval auto name() {return string_view{str.value};}

TEST_CASE("String literal", "[endpoints][string_literal]")
{
    REQUIRE(string_view(string_literal{"Hello world"}.value) == "Hello world");
    REQUIRE(name<"test">() == "test");
}

struct struct_with_name : name_<"foo"> {};
TEST_CASE("name_", "[endpoints][bases][name_]")
{
    REQUIRE(string_view(struct_with_name::name()) == string_view("foo"));
}
