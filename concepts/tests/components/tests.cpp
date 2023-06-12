#include <type_traits>
#include <string>
#include <catch2/catch_test_macros.hpp>
#include "helpers/metadata.hpp"
#include "helpers/endpoints.hpp"
#include "concepts/components.hpp"

using namespace sygaldry::concepts;
using namespace sygaldry::helpers;
using std::string;

struct regular_component_t : name_<"regular component">
{
    struct inputs_t {} inputs;
    struct outputs_t {} outputs;
    struct parts_t {} parts;
    static void main(const inputs_t&, outputs_t&) {}
} regular_component;

static_assert(has_main_subroutine<regular_component_t>);
static_assert(has_inputs<regular_component_t>);
static_assert(has_outputs<regular_component_t>);
static_assert(has_parts<regular_component_t>);
static_assert(Component<regular_component_t>);
static_assert(Component<regular_component_t>);
static_assert(not Component<bng<"a bang">>);
struct container_component_t
{
    static _consteval auto name() {return "container component";}
    regular_component_t component1;
} container_component;

static_assert(SimpleAggregate<container_component_t>);
static_assert(ComponentContainer<container_component_t>);
//static_assert(Component<container_component_t>);
// docs say: aggregates may not have base classes
struct not_simple_aggregate1 : name_<"foo"> { };
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
static_assert(SimpleAggregate<regular_component_t::inputs_t>);
static_assert(SimpleAggregate<regular_component_t::outputs_t>);
static_assert(SimpleAggregate<regular_component_t::parts_t>);
static_assert(SimpleAggregate<container_component_t>);
static_assert(std::same_as<regular_component_t::inputs_t&, decltype(inputs_of(regular_component))>);
static_assert(std::same_as<regular_component_t::outputs_t&, decltype(outputs_of(regular_component))>);
static_assert(std::same_as<regular_component_t::parts_t&, decltype(parts_of(regular_component))>);
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
struct dummy_component {
    struct inputs_t {
        struct ep1_t : name_<"ep1">, persistent<float>
        {
            float extra_value;
        } ep1;
    } inputs;

    struct outputs_t {
        struct ep2_t : name_<"ep2">, persistent<float>
        {
            float another_extra;
        } ep2;
    } outputs;

    struct parts_t {
        struct dummy_part : name_<"dp"> {
            struct parts_t {
                static _consteval auto name() {return "dpp";}
            } parts;
            void main() {};
        } part;
    } parts;

    void main() {}
};

struct accessor_test_container_t
{
    static _consteval auto name() {return "container";}
    struct c1_t : dummy_component, name_<"c1"> {} c1;
    struct c2_t : dummy_component, name_<"c2"> {} c2;
} accessor_test_container;

static_assert(Component<accessor_test_container_t::c1_t>);
static_assert(Component<accessor_test_container_t::c2_t>);
static_assert(Component<dummy_component::parts_t::dummy_part>);
static_assert(ComponentContainer<accessor_test_container_t>);

TEST_CASE("for each X")
{
    string allnames{};
    auto add_names = [&](auto& entity)
    {
        allnames += string(entity.name());
    };

    SECTION("for each component")
    {
        for_each_component(accessor_test_container, add_names);
        REQUIRE(allnames == string("c1dpc2dp"));
    }

    SECTION("for each endpoint")
    {
        for_each_endpoint(accessor_test_container, add_names);
        REQUIRE(allnames == string("ep1ep2ep1ep2"));
    }

    SECTION("for each input")
    {
        for_each_input(accessor_test_container, add_names);
        REQUIRE(allnames == string("ep1ep1"));
    }

    SECTION("for each output")
    {
        for_each_output(accessor_test_container, add_names);
        REQUIRE(allnames == string("ep2ep2"));
    }

    SECTION("for each node")
    {
        string allnodes{};
        auto add_node = [&]<typename T>(T& entity, auto tag)
        {
            if constexpr (has_name<T>) allnodes += string(entity.name());
        };
        for_each_node(accessor_test_container, add_node);
        REQUIRE(allnodes == string("containerc1ep1ep2dpdppc2ep1ep2dpdpp"));
    }
}
