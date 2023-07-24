/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "bindings/osc_match_pattern.hpp"

TEST_CASE("OSC match wildcards")
{
    CHECK(osc_match_pattern("/???", "/123"));
    CHECK(osc_match_pattern("/foo.?", "/foo.8"));
    CHECK(not osc_match_pattern("/foo.?", "/foo.42"));
}
TEST_CASE("OSC match globs")
{
    CHECK(osc_match_pattern("/*", "/123"));
    CHECK(not osc_match_pattern("/*", "/123/456"));
    CHECK(osc_match_pattern("/foo.*", "/foo.42"));
    CHECK(osc_match_pattern("/foo.*", "/foo."));
    CHECK(osc_match_pattern("/*.bar", "/.bar"));
    CHECK(osc_match_pattern("/*.bar", "/foo.bar"));
    CHECK(osc_match_pattern("/*.bar", "/baz.bar"));
    CHECK(osc_match_pattern("/*/123", "/banana/123"));
    CHECK(osc_match_pattern("/**456", "/123456"));
    CHECK(not osc_match_pattern("/**456", "/foo.bar")); // two asterisks should not form a super-glob
    CHECK(not osc_match_pattern("/*/*", "/banana"));
    CHECK(not osc_match_pattern("/banana/*", "/apple/pie"));
}
TEST_CASE("OSC match sets")
{
    CHECK(osc_match_pattern("/[123]23", "/123"));
    CHECK(osc_match_pattern("/[123]23", "/223"));
    CHECK(osc_match_pattern("/[123]23", "/323"));
    CHECK(not osc_match_pattern("/[123]23", "/423"));

    CHECK(osc_match_pattern("/[[123]23", "/323")); // by sheer dumb luck
    CHECK(not osc_match_pattern("/[123]]23", "/323"));
    CHECK(not osc_match_pattern("/[12323", "/323"));
    CHECK(not osc_match_pattern("/123]23", "/323"));
}
TEST_CASE("OSC match ranges")
{
    CHECK(osc_match_pattern("/[1-9]", "/1"));
    CHECK(osc_match_pattern("/[1-9]", "/5"));
    CHECK(osc_match_pattern("/[1-9]", "/9"));
    CHECK(osc_match_pattern("/[9-1]", "/5"));
    CHECK(not osc_match_pattern("/[1-9]", "/a"));
    CHECK(osc_match_pattern("/[-1]", "/-"));
    CHECK(osc_match_pattern("/[-1]", "/1"));
    CHECK(osc_match_pattern("/[1-]", "/-"));
    CHECK(osc_match_pattern("/[1-]", "/1"));
    CHECK(not osc_match_pattern("/[-1]", "/2"));
    CHECK(not osc_match_pattern("/[1-]", "/2"));
}
TEST_CASE("OSC match inverted sets")
{
    CHECK(not osc_match_pattern("/[!123]23", "/123"));
    CHECK(not osc_match_pattern("/[!123]23", "/223"));
    CHECK(not osc_match_pattern("/[!123]23", "/323"));
    CHECK(osc_match_pattern("/[!123]23", "/423"));
    CHECK(not osc_match_pattern("/[!1-9]", "/1"));
    CHECK(not osc_match_pattern("/[!1-9]", "/5"));
    CHECK(not osc_match_pattern("/[!1-9]", "/9"));
    CHECK(osc_match_pattern("/[!1-9]", "/a"));
    CHECK(not osc_match_pattern("/[!-1]", "/-"));
    CHECK(not osc_match_pattern("/[!-1]", "/1"));
    CHECK(not osc_match_pattern("/[!1-]", "/-"));
    CHECK(not osc_match_pattern("/[!1-]", "/1"));
    CHECK(osc_match_pattern("/[!-1]", "/2"));
    CHECK(osc_match_pattern("/[!1-]", "/2"));
    CHECK(osc_match_pattern("/[whatever!]", "/!"));
}
TEST_CASE("OSC match substrings")
{
    CHECK(osc_match_pattern("/{apple,banana,blueberry}/pie", "/apple/pie"));
    CHECK(osc_match_pattern("/{apple,banana,blueberry}/pie", "/banana/pie"));
    CHECK(osc_match_pattern("/{apple,banana,blueberry}/pie", "/blueberry/pie"));
    CHECK(osc_match_pattern("/{{apple,banana,blueberry}/pie", "/banana/pie"));
    CHECK(not osc_match_pattern("/{apple,banana,blueberry}/pie", "/sugar/pie"));

    CHECK(osc_match_pattern("/{{apple,banana,blueberry}/pie", "/banana/pie")); // sheer dumb luck
    CHECK(not osc_match_pattern("/{apple,banana,blueberry}}/pie", "/banana/pie"));
    CHECK(not osc_match_pattern("/{apple,banana,blueberry/pie", "/banana/pie"));
}
TEST_CASE("OSC match regular")
{
    CHECK(osc_match_pattern("/sugar/pie", "/sugar/pie"));
    CHECK(not osc_match_pattern("/sugar/pie", "/apple/pie"));
}
