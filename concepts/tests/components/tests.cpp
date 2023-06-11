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
// these should both count as components, for now
struct struct_with_void_main_method { void main() {}; };
struct struct_with_void_operator_call { void operator()() {}; };

// and these should not
struct struct_with_int_main_method { int main(int i) { return i + 1; }; };
struct struct_with_int_operator_call { int operator()(int i) { return i + 1; }; };
using void_operator = decltype(&struct_with_void_operator_call::operator());
using void_operator_reflection = func_reflection<void_operator>;
using void_return_type = void_operator_reflection::ret;
static_assert(std::is_same_v<void_return_type, void>);

using int_operator = decltype(&struct_with_int_operator_call::operator());
using int_operator_reflection = func_reflection<int_operator>;
using int_return_type = int_operator_reflection::ret;
static_assert(std::is_same_v<int_return_type, int>);
static_assert(std::is_same_v<int_return_type, int>);
static_assert(std::is_same_v<func_ret_t<&struct_with_int_operator_call::operator()>, int>);

static_assert(func_reflectable<void_operator>);
static_assert(func_reflectable<int_operator>);
static_assert(has_main_subroutine<struct_with_void_main_method>);
static_assert(!has_main_subroutine<struct_with_int_main_method>);
static_assert(has_main_subroutine<struct_with_void_operator_call>);
static_assert(!has_main_subroutine<struct_with_int_operator_call>);
