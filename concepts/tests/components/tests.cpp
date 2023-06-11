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
} my_component;

static_assert(has_main_subroutine<my_component_t>);
static_assert(has_inputs<my_component_t>);
static_assert(has_outputs<my_component_t>);
static_assert(has_parts<my_component_t>);
static_assert(RegularComponent<my_component_t>);
static_assert(Component<my_component_t>);
struct struct_of_components
{
    my_component_t component;
} my_assembly1;

struct component_of_parts : name_<"component concept test assembly">
{
    struct parts_t {
        my_component_t component;
    } parts;
} my_assembly2;

static_assert(is_simple_aggregate<struct_of_components>);
static_assert(has_only_components<struct_of_components>);
static_assert(PureAssembly<struct_of_components>);
//static_assert(Component<struct_of_components>);

static_assert(has_parts<component_of_parts>);
static_assert(not has_main_subroutine<component_of_parts>);
static_assert(not has_inputs<component_of_parts>);
static_assert(not has_outputs<component_of_parts>);
static_assert(PureAssembly<component_of_parts>);
//static_assert(Component<component_of_parts>);
static_assert(std::same_as<my_component_t::inputs_t&, decltype(inputs_of(my_component))>);
static_assert(std::same_as<my_component_t::outputs_t&, decltype(outputs_of(my_component))>);
static_assert(std::same_as<my_component_t::parts_t&, decltype(parts_of(my_component))>);
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
