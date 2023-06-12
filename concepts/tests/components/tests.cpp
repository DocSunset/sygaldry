#include <type_traits>
#include <map>
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
struct component_of_parts : name_<"component concept test assembly">
{
    struct parts_t {
        my_component_t component;
    } parts;
} my_assembly2;

struct struct_of_components
{
    my_component_t component;
} my_assembly1;

static_assert(has_parts<component_of_parts>);
static_assert(not has_main_subroutine<component_of_parts>);
static_assert(not has_inputs<component_of_parts>);
static_assert(not has_outputs<component_of_parts>);
static_assert(PureAssembly<component_of_parts>);
//static_assert(Component<component_of_parts>);

static_assert(Aggregate<struct_of_components>);
static_assert(has_only_components<struct_of_components>);
static_assert(PureAssembly<struct_of_components>);
//static_assert(Component<struct_of_components>);
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
static_assert(not has_main_subroutine<member_main>);
static_assert(not has_main_subroutine<int_main>);
static_assert(not has_main_subroutine<int_operator>);
// docs say: aggregates may not have base classes
struct foo { int x; foo(int a, int b) : x{a+b} {} };
struct not_simple_aggregate1 : foo { };
static_assert(std::is_aggregate_v<not_simple_aggregate1>); // passes
// auto failure = boost::pfr::tuple_size_v<not_simple_aggregate1>; // static assertion failure

// docs say: aggregates may not have const fields
struct not_simple_aggregate2 { const int i; };
static_assert(std::is_aggregate_v<not_simple_aggregate2>); // passes
static_assert(1 == boost::pfr::tuple_size_v<not_simple_aggregate2>); // works fine, even though the docs say it's not allowed

// docs say: aggregates may not have reference fields
struct not_simple_aggregate3 { int& i; };
static_assert(std::is_aggregate_v<not_simple_aggregate3>); // passes
static_assert(1 == boost::pfr::tuple_size_v<not_simple_aggregate3>); // works fine, even though the docs say it's not allowed

// docs say: aggregates may not have c arrays
struct not_simple_aggregate4 { float f; int i[5]; };
static_assert(std::is_aggregate_v<not_simple_aggregate4>); // passes
static_assert(6 == boost::pfr::tuple_size_v<not_simple_aggregate4>); // works fine, even though the docs say it's not allowed, although arguably this should return 2?

// docs say: aggregates may not have constructors
struct not_simple_aggregate5 { float f; not_simple_aggregate5(float a, float b) : f{a + b} {} };
static_assert(not std::is_aggregate_v<not_simple_aggregate5>); // a class with a constructor is not aggregate
//auto failure = boost::pfr::tuple_size_v<not_simple_aggregate5>; // static assertion failure

// docs don't mention scalars, but implementation suggests they're fine
using not_simple_aggregate6 = float;
static_assert(not std::is_aggregate_v<not_simple_aggregate6>); // a float is not aggregate
static_assert(std::is_scalar_v<not_simple_aggregate6>); // a float is scalar
static_assert(1 == boost::pfr::tuple_size_v<not_simple_aggregate6>); // works, in accordance with the docs

// docs say: unions not allowed
union not_simple_aggregate7 {float f; int i;} nope;
static_assert(std::is_aggregate_v<not_simple_aggregate7>); // passes
// auto failure = boost::pfr::structure_to_tuple(nope); // static assertion failure

static_assert(Aggregate<component_of_parts::parts_t>);
static_assert(Aggregate<my_component_t::inputs_t>);
static_assert(Aggregate<my_component_t::outputs_t>);
