#include <catch2/catch_test_macros.hpp>

#include "basic_logger.hpp"
#include "test_putter.hpp"
#include <iostream>
#include <sstream>
#include <string>

using namespace bindings::basic_logger;

TEST_CASE("BasicLogger print") {
    BasicLogger<TestPutter> logger;

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
