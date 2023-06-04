#include <string_view>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"

using std::string_view;
using namespace sygaldry::spelling;

TEST_CASE("Snake case", "[utilities][metadata][names][snake]")
{
    SECTION("Snake Case Function-like Access")
    {
        struct example_t
        {
            static _consteval const char * name() {return "snake case example";}
        } x;

        REQUIRE(string_view(snake_case(x)) == string_view("snake_case_example"));
    }

    SECTION("Snake Case Template Variable Access")
    {
        struct example_t
        {
            static _consteval const char * name() {return "snake case example";}
        };

        REQUIRE(string_view(snake_case_v<example_t>) == string_view("snake_case_example"));
    }
}

TEST_CASE("Kebab Case")
{
    struct example_t
    {
        static _consteval const char * name() {return "kebab case example";}
    } x;

    CHECK(string_view(kebab_case(x)) == string_view("kebab-case-example"));
    CHECK(string_view(kebab_case_v<example_t>) == string_view("kebab-case-example"));
}

TEST_CASE("Upper/lower")
{
    struct example_t
    {
        static _consteval const char * name() {return "A Basic Example";}
    } x;
    CHECK(string_view(upper_snake_case(x)) == string_view("A_BASIC_EXAMPLE"));
    CHECK(string_view(lower_kebab_case(x)) == string_view("a-basic-example"));
}
