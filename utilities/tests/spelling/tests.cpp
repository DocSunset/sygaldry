#include <string_view>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include "utilities/consteval.hpp"
#include "utilities/spelling.hpp"
#include "components/metadata.hpp"

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

TEST_CASE("Spelling with helpers")
{
    using namespace sygaldry::metadata;
    struct example_t : name_<"Helper Example"> {} x;
    CHECK(string_view(upper_snake_case(x)) == string_view("HELPER_EXAMPLE"));
    CHECK(string_view(lower_kebab_case(x)) == string_view("helper-example"));
    CHECK(string_view(upper_kebab_case(x)) == string_view("HELPER-EXAMPLE"));
    CHECK(string_view(lower_snake_case(x)) == string_view("helper_example"));
}

TEST_CASE("Passthrough spelling")
{
    struct example_t
    {
        static _consteval const char * name() {return "A Basic Example";}
    } x;
    REQUIRE(string_view(passthrough_spelling(x)) == string_view("A Basic Example"));
}
