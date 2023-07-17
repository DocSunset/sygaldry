#include <catch2/catch_test_macros.hpp>

#include "test_logger.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <array>

using namespace sygaldry::bindings;

TEST_CASE("BasicLogger print") {
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
