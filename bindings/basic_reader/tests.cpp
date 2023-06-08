#include <catch2/catch_test_macros.hpp>

#include "test_reader.hpp"

using namespace sygaldry::bindings::basic_reader;

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
