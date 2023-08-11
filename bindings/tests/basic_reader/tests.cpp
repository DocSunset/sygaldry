/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>

#include "bindings/test_reader.hpp"

using namespace sygaldry::bindings;

TEST_CASE("TestReader")
{
    TestReader reader{};
    reader.ss.str("Hello world!\n");

    REQUIRE(reader.ready());
    REQUIRE(reader.getchar() == 'H');

    char last_char = 'x';
    while (reader.ready())
    {
        last_char = reader.getchar();
    }

    REQUIRE(last_char == '\n');
}
