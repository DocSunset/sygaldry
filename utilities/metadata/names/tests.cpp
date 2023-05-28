#include <catch2/catch_test_macros.hpp>
#include "names.hpp"
#include <string>
#include <array>

using std::string;
using namespace utilities::metadata::names;

TEST_CASE("Snake case", "[utilities][metadata][names][snake]")
{
    SECTION("Snake Case Function-like Access")
    {
        struct example_t
        {
            static consteval const char * name() {return "snake case example";}
        } x;

        REQUIRE(string(snake_case(x)) == string("snake_case_example"));
    }
    SECTION("Snake Case Template Variable Access")
    {
        struct example_t
        {
            static consteval const char * name() {return "snake case example";}
        };

        REQUIRE(string(snake_case_v<example_t>) == string("snake_case_example"));
    }
}
TEST_CASE("Kebab Case")
{
    struct example_t
    {
        static consteval const char * name() {return "kebab case example";}
    } x;

    CHECK(string(kebab_case(x)) == string("kebab-case-example"));
    CHECK(string(kebab_case_v<example_t>) == string("kebab-case-example"));
}
TEST_CASE("Upper/lower")
{
    struct example_t
    {
        static consteval const char * name() {return "A Basic Example";}
    } x;
    CHECK(string(upper_snake_case(x)) == string("A_BASIC_EXAMPLE"));
    CHECK(string(lower_kebab_case(x)) == string("a-basic-example"));
}
