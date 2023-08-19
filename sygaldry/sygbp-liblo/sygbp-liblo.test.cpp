/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <string>
#include <catch2/catch_test_macros.hpp>
#include "sygac-components.hpp"
#include "sygah-endpoints.hpp"
#include "sygbp-test_component.hpp"
#include "sygbp-liblo.hpp"

using std::string;

using namespace sygaldry;
using namespace sygaldry::bindings;
using namespace sygaldry::components;

TEST_CASE("liblo osc port is valid")
{
    LibloOsc<TestComponent> osc;
    text_message<"s1"> s1{};
    text_message<"s2"> s2{string("7777")};
    text_message<"s3"> s3{string("77777")};
    CHECK(not osc.port_is_valid(s1));
    CHECK(osc.port_is_valid(s2));
    CHECK(not osc.port_is_valid(s3));
}
