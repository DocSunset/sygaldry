/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>

#include "sygup-test_logger.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <array>

using namespace sygaldry::utility_components;


TEST_CASE("sygaldry BasicLogger print") {
    TestLogger logger;
    logger.put.ss.str("");

    SECTION("Printing integers")
    {
        logger.print(42);
        REQUIRE(logger.put.ss.str() == "42");
    }

    SECTION("Printing floating-point numbers")
    {
        logger.print(-2.71828);
        REQUIRE(logger.put.ss.str() == "-2.71828");
    }

    SECTION("Printing strings")
    {
        logger.print("Hello world!");
        REQUIRE(logger.put.ss.str() == "Hello world!");
    }

    SECTION("Printing arrays")
    {
        logger.print(std::array<int, 3>{1,2,3});
        REQUIRE(logger.put.ss.str() == "[1 2 3]");
    }

    SECTION("Variadic print")
    {
        logger.print("1", 2, " ", 3.14159);
        REQUIRE(logger.put.ss.str() == "12 3.14159");
    }

    SECTION("Empty println")
    {
        logger.println();
        REQUIRE(logger.put.ss.str() == "\n");
    }
}
