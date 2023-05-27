#include <catch2/catch_test_macros.hpp>

#include "bindings/basic_logger/basic_logger.hpp"
#include <iostream>
#include <sstream>
#include <string>

using namespace bindings::basic_logger;

struct TestPutter
{
    std::stringstream ss;
    void operator()(char c)
    {
        ss << c;
    }
};


TEST_CASE("BasicLogger print") {
    BasicLogger<TestPutter> logger;

    SECTION("Printing integers") {
        logger.print(42);
        REQUIRE(logger.put.ss.str() == "42");
        logger.print(-123);
        REQUIRE(logger.put.ss.str() == "42-123");
    }

    SECTION("Printing floating-point numbers") {
        logger.print(3.14159);
        REQUIRE(logger.put.ss.str() == "3.14159");

        logger.print(-2.71828);
        REQUIRE(logger.put.ss.str() == "3.14159-2.71828");
    }

    SECTION("Printing strings") {
        logger.print("Hello, ");
        REQUIRE(logger.put.ss.str() == "Hello, ");

        logger.print(std::string_view("world!"));
        REQUIRE(logger.put.ss.str() == "Hello, world!");
    }
}
