/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <string_view>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include "sygah-consteval.hpp"
#include "sygah-endpoints.hpp"
#include "sygbp-osc_string_constants.hpp"

using std::string_view;
using namespace sygaldry;
using namespace sygaldry::sygbp;

TEST_CASE("sygaldry osc path")
{
    struct root_t { static _consteval const char * name() {return "Root";} };
    struct leaf_t { static _consteval const char * name() {return "leaf";} };
    using Path = tpl::tuple<root_t, leaf_t>;
    CHECK(string_view(osc_path<Path>::value.data()) == string_view("/Root/leaf"));
}
TEST_CASE("sygaldry osc type tag string")
{
    CHECK(string_view(osc_type_string_v<button<"test button">>)             == string_view(",i"));
    CHECK(string_view(osc_type_string_v<toggle<"test toggle">>)             == string_view(",i"));
    CHECK(string_view(osc_type_string_v<text<"test text">>)                 == string_view(",s"));
    CHECK(string_view(osc_type_string_v<text_message<"test text message">>) == string_view(",s"));
    CHECK(string_view(osc_type_string_v<slider<"test slider">>)             == string_view(",f"));
    CHECK(string_view(osc_type_string_v<bng<"test bang">>)                  == string_view(","));
    CHECK(string_view(osc_type_string_v<array<"test array", 3>>)            == string_view(",fff"));
}
