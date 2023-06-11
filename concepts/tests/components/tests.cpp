#include <type_traits>
#include <catch2/catch_test_macros.hpp>
#include "helpers/metadata.hpp"
#include "concepts/components.hpp"

using namespace sygaldry::concepts;
using namespace sygaldry::helpers;


struct my_component_t : name_<"component concept test component">
{
    struct inputs_t {} inputs;
    struct outputs_t {} outputs;
    struct parts_t {} parts;
    static void main(const inputs_t&, outputs_t&) {}
};

struct struct_of_components
{
    my_component_t component;
};

TEST_CASE("Base Component Concepts")
{
    SECTION("has ___")
    {
        // static_assert(has_main_subroutine<my_component_t>);
        // static_assert(has_inputs<my_component_t>);
        // static_assert(has_outputs<my_component_t>);
        // static_assert(has_parts<my_component_t>);
        // static_assert(has_parts<my_component_t>);
        // TODO static_assert(has_throughpoints<my_component_t>);
        // TODO static_assert(has_plugins<my_component_t>);
        // static_assert(has_only_endpoints<struct_of_components>);
        // static_assert(is_simple_aggregate<struct_of_components>);
    }
}
struct void_main { void main() {} };
struct void_operator { void operator()() {} };
static_assert(has_main_subroutine<void_main>);
static_assert(has_main_subroutine<void_operator>);

struct member_main { int main; };
struct int_main { int main() {return 1;} };
struct int_operator { int operator()() {return 1;} };
static_assert(!has_main_subroutine<member_main>);
static_assert(!has_main_subroutine<int_main>);
static_assert(!has_main_subroutine<int_operator>);
