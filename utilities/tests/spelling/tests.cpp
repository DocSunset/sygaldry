#include <string_view>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"

using std::string_view;
using namespace sygaldry::utilities::spelling;

TEST_CASE("Snake Case", "[utilities][names][snake]")
{
    REQUIRE(string_view(snake_case<"snake case example">()) == string_view("snake_case_example"));
}

TEST_CASE("Kebab Case")
{
    CHECK(string_view(kebab_case<"kebab case example">()) == string_view("kebab-case-example"));
}

TEST_CASE("Upper/lower")
{
    CHECK(string_view(upper_snake_case<"a basic example">()) == string_view("A_BASIC_EXAMPLE"));
    CHECK(string_view(lower_kebab_case<"A Title Perhaps">()) == string_view("a-title-perhaps"));
}
