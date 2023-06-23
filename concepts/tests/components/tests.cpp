#include <type_traits>
#include <string>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "helpers/metadata.hpp"
#include "helpers/endpoints.hpp"
#include "concepts/components.hpp"

using namespace sygaldry;
using std::string;

struct regular_component_t : name_<"regular component">
{
    struct inputs_t {} inputs;
    struct outputs_t {} outputs;
    struct parts_t {
        struct subcomponent_t : name_<"a subcomponent"> {
            struct inputs_t {} inputs;
            void operator()() {}
        } subcomponent;
    } parts;
    static void main(const inputs_t&, outputs_t&) {}
} regular_component;

static_assert(has_main_subroutine<regular_component_t>);
static_assert(has_inputs<regular_component_t>);
static_assert(has_outputs<regular_component_t>);
static_assert(has_parts<regular_component_t>);
static_assert(Component<regular_component_t>);
static_assert(Component<regular_component_t>);

struct container_component_t
{
    static _consteval auto name() {return "container component";}
    regular_component_t component1;
} container_component;

static_assert(SimpleAggregate<container_component_t>);
static_assert(ComponentContainer<container_component_t>);
static_assert(not Component<container_component_t>);

static_assert(not ComponentContainer<regular_component_t>);
static_assert(ComponentContainer<regular_component_t::parts_t>);
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
static_assert(std::same_as<regular_component_t::inputs_t&, decltype(inputs_of(regular_component))>);
static_assert(std::same_as<regular_component_t::outputs_t&, decltype(outputs_of(regular_component))>);
static_assert(std::same_as<regular_component_t::parts_t&, decltype(parts_of(regular_component))>);

static_assert(std::same_as<regular_component_t::inputs_t, type_of_inputs_t<regular_component_t>>);
static_assert(std::same_as<regular_component_t::outputs_t, type_of_outputs_t<regular_component_t>>);
static_assert(std::same_as<regular_component_t::parts_t, type_of_parts_t<regular_component_t>>);
struct almost_container
{
    float nope;
    regular_component_t yep;
};
static_assert(not ComponentContainer<almost_container>);
static_assert(ComponentContainer<regular_component_t::parts_t>);
static_assert(ComponentContainer<container_component_t>);
struct c1_t : name_<"c1"> {
    struct inputs_t {
        struct in1_t : name_<"in1">, persistent<float>
        {
            float extra_value;
        } in1;
        struct in2_t : name_<"in2">, persistent<float>
        {
            float extra_value;
        } in2;
    } inputs;

    struct outputs_t {
        struct out_t : name_<"out">, persistent<float>
        {
            float another_extra;
        } out;
    } outputs;

    struct parts_t {
        struct dummy_part : name_<"dp"> {
            struct parts_t {
                static _consteval auto name() {return "dpp";}
            } parts;
            void main() {};
        } part;
    } parts;

    void main(){}
};

struct c2_t : name_<"c2"> {
    struct inputs_t {
        struct in1_t : name_<"in1">, persistent<float>
        {
            float extra_value;
        } in1;
        struct in2_t : name_<"in2">, persistent<float>
        {
            float extra_value;
        } in2;
    } inputs;

    struct outputs_t {
        struct out_t : name_<"out">, persistent<float>
        {
            float another_extra;
        } out;
    } outputs;

    struct parts_t {
        struct dummy_part : name_<"dp"> {
            struct parts_t {
                static _consteval auto name() {return "dpp";}
            } parts;
            void main() {};
        } part;
    } parts;

    void main(){}
};

struct accessor_test_container_t
{
    c1_t c1;
    c2_t c2;
};

constinit accessor_test_container_t accessor_test_container{};

static_assert(Component<c1_t>);
static_assert(Component<c2_t>);
static_assert(Component<c1_t::parts_t::dummy_part>);
static_assert(ComponentContainer<accessor_test_container_t>);
using atc   = accessor_test_container_t;
using c1    =     c1_t;
using ic1   =          c1::inputs_t;
using in11  =              ic1::in1_t;
using in21  =              ic1::in2_t;
using oc1   =          c1::outputs_t;
using out1  =              oc1::out_t;
using pc1   =          c1::parts_t;
using dp1   =              pc1::dummy_part;
using dppc1 =                  dp1::parts_t;
using c2    =     c2_t;
using ic2   =          c2::inputs_t;
using in12  =              ic2::in1_t;
using in22  =              ic2::in2_t;
using oc2   =          c2::outputs_t;
using out2  =              oc2::out_t;
using pc2   =          c2::parts_t;
using dp2   =              pc2::dummy_part;
using dppc2 =                  dp2::parts_t;

static_assert(std::same_as<decltype(component_to_tree(accessor_test_container))
, std::tuple< tagged<node::component_container,atc>
            , std::tuple< tagged<node::component,c1>
                        , std::tuple< tagged<node::inputs_container,ic1>
                                    , std::tuple<tagged<node::input_endpoint,in11>>
                                    , std::tuple<tagged<node::input_endpoint,in21>>
                                    >
                        , std::tuple< tagged<node::outputs_container,oc1>
                                    , std::tuple<tagged<node::output_endpoint,out1>>
                                    >
                        , std::tuple< tagged<node::parts_container,pc1>
                                    , std::tuple< tagged<node::part_component,dp1>
                                                , std::tuple<tagged<node::parts_container,dppc1>>
                                                >
                                    >
                        >
            , std::tuple< tagged<node::component,c2>
                        , std::tuple< tagged<node::inputs_container,ic2>
                                    , std::tuple<tagged<node::input_endpoint,in12>>
                                    , std::tuple<tagged<node::input_endpoint,in22>>
                                    >
                        , std::tuple< tagged<node::outputs_container,oc2>
                                    , std::tuple<tagged<node::output_endpoint,out2>>
                                    >
                        , std::tuple< tagged<node::parts_container,pc2>
                                    , std::tuple< tagged<node::part_component,dp2>
                                                , std::tuple<tagged<node::parts_container,dppc2>>
                                                >
                                    >
                        >
            >
>);

TEST_CASE("component_to_tree")
{
    constexpr auto tree = component_to_tree(accessor_test_container);
    auto& in1 = std::get<0>(std::get<1>(std::get<1>(std::get<1>(tree)))).ref;
    accessor_test_container.c1.inputs.in1.extra_value = 0.0;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 0.0);
    in1.extra_value = 3.14f;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 3.14f);
}
TEST_CASE("tuple head and tail")
{
    struct {
        int a;
        int b;
        int c;
    } x = {0,0,0};

    auto tup = boost::pfr::structure_tie(x);
    auto head = tuple_head(tup);
    static_assert(std::same_as<int, decltype(head)>);
    auto tail = tuple_head(tup);

    auto empty_tuple = std::tuple<>{};
    auto empty = tuple_head(empty_tuple);
}
TEST_CASE("component_tree_to_node_list")
{
    constexpr auto flattened = component_tree_to_node_list(component_to_tree(accessor_test_container));
    static_assert(std::tuple_size_v<decltype(flattened)> == std::tuple_size_v<std::tuple<atc, c1, ic1, in11, in21, oc1, out1, pc1, dp1, dppc1, c2, ic2, in12, in22, oc2, out2, pc2, dp2, dppc2>>);

    auto& in1 = std::get<3>(flattened).ref;
    accessor_test_container.c1.inputs.in1.extra_value = 0.0;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 0.0);
    in1.extra_value = 3.14f;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 3.14f);
}
TEST_CASE("node_list_filter")
{
    constexpr auto& in1 = find<in11>(component_tree_to_node_list(component_to_tree(accessor_test_container)));
    accessor_test_container.c1.inputs.in1.extra_value = 0.0;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 0.0);
    in1.extra_value = 3.14f;
    REQUIRE(accessor_test_container.c1.inputs.in1.extra_value == 3.14f);
}
auto in11_path = path_of<in11>(component_to_tree(accessor_test_container));
static_assert(std::same_as< std::remove_cvref_t<decltype(in11_path)>
                          , std::tuple< tagged<node::component,c1>
                                      , tagged<node::input_endpoint,in11>
                                      >
                          >);

struct deep_component : name_<"root"> {
    void main();
    struct parts_t {
        struct n1 : name_<"n1"> {
            void main();
            struct parts_t {
                struct n2 : name_<"n2"> {
                    void main();
                    struct parts_t {
                        struct n3 : name_<"n3"> {
                            void main();
                            struct inputs_t {
                                struct in : name_<"in"> {} input;
                            } inputs;
                        } part;
                    } parts;
                } part;
            } parts;
        } part;
    } parts;
} deep;

using deep_input = deep_component::parts_t::n1::parts_t::n2::parts_t::n3::inputs_t::in;
auto deep_path = path_of<deep_input>(deep);
static_assert(std::same_as< std::remove_cvref_t<decltype(deep_path)>
        , std::tuple< tagged<node::component,deep_component>
                    , tagged<node::part_component,deep_component::parts_t::n1>
                    , tagged<node::part_component,deep_component::parts_t::n1::parts_t::n2>
                    , tagged<node::part_component,deep_component::parts_t::n1::parts_t::n2::parts_t::n3>
                    , tagged<node::input_endpoint,deep_component::parts_t::n1::parts_t::n2::parts_t::n3::inputs_t::in>
                    >
        >);

auto outputs = remove_node_tags(node_list_filter_by_tag<node::output_endpoint>(component_tree_to_node_list(component_to_tree(accessor_test_container))));
static_assert(std::same_as<decltype(outputs), std::tuple<out1, out2>>);
static_assert(std::same_as<decltype(outputs), output_endpoints_t<accessor_test_container_t>>);
static_assert(std::same_as<decltype(remove_node_tags(deep_path)), path_t<deep_input, deep_component>>);
// TODO: test the other ones as needed
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
        REQUIRE(allnames == string("in1in2outin1in2out"));
    }

    SECTION("for each input")
    {
        for_each_input(accessor_test_container, add_names);
        REQUIRE(allnames == string("in1in2in1in2"));
    }

    SECTION("for each output")
    {
        for_each_output(accessor_test_container, add_names);
        REQUIRE(allnames == string("outout"));

        BENCHMARK("for each output bench")
        {
            for_each_output(accessor_test_container, add_names);
            return allnames;
        };
    }

    SECTION("for each output in list (pregen)")
    {
        string allnodes{};
        auto add_node = [&]<typename T>(T& entity, auto tag)
        {
            if constexpr (has_name<T>) allnodes += string(entity.name());
        };
        constexpr auto filtered = node_list_filter_by_tag<node::output_endpoint>(
                component_tree_to_node_list(component_to_tree(accessor_test_container)));
        for_each_node_in_list(filtered, add_node);
        REQUIRE(allnodes == string("outout"));

        BENCHMARK("for each output in list (pregen) bench")
        {
            string allnodes{};
            for_each_node_in_list(filtered, add_node);
            return allnodes;
        };

        BENCHMARK("for each output in list bench (from component)")
        {
            string allnodes{};
            constexpr auto filtered = node_list_filter_by_tag<node::output_endpoint>(
                    component_tree_to_node_list(component_to_tree(accessor_test_container)));
            for_each_node_in_list(filtered, add_node);
            return allnodes;
        };
    }

    SECTION("for each output in list")
    {
        constexpr auto list = component_tree_to_node_list(component_to_tree(accessor_test_container));
    }

    SECTION("for each node")
    {
        string allnodes{};
        auto add_node = [&]<typename T>(T& entity, auto tag)
        {
            if constexpr (has_name<T>) allnodes += string(entity.name());
        };
        for_each_node(accessor_test_container, add_node);
        REQUIRE(allnodes == string("c1in1in2outdpdppc2in1in2outdpdpp"));

        BENCHMARK("for each node bench")
        {
            string allnodes{};
            auto add_node = [&]<typename T>(T& entity, auto tag)
            {
                if constexpr (has_name<T>) allnodes += string(entity.name());
            };
            for_each_node(accessor_test_container, add_node);
            return allnodes;
        };
    }

    SECTION("for each node in list (pregen)")
    {
        constexpr auto list = component_tree_to_node_list(component_to_tree(accessor_test_container));
        string allnodes{};
        auto add_node = [&]<typename T>(T& entity, auto tag)
        {
            if constexpr (has_name<T>) allnodes += string(entity.name());
        };
        for_each_node_in_list(list, add_node);
        REQUIRE(allnodes == string("c1in1in2outdpdppc2in1in2outdpdpp"));

        BENCHMARK("for each node in list (pregen) bench")
        {
            string allnodes{};
            auto add_node = [&]<typename T>(T& entity, auto tag)
            {
                if constexpr (has_name<T>) allnodes += string(entity.name());
            };
            for_each_node_in_list(list, add_node);
            return allnodes;
        };
    }

    SECTION("for each node in list (pregen)")
    {
        string allnodes{};
        constexpr auto list = component_tree_to_node_list(component_to_tree(accessor_test_container));
        auto add_node = [&]<typename T>(T& entity, auto tag)
        {
            if constexpr (has_name<T>) allnodes += string(entity.name());
        };
        for_each_node_in_list(list, add_node);
        REQUIRE(allnodes == string("c1in1in2outdpdppc2in1in2outdpdpp"));

        BENCHMARK("for each node in list (pregen) bench")
        {
            constexpr auto list = component_tree_to_node_list(component_to_tree(accessor_test_container));
            string allnodes{};
            auto add_node = [&]<typename T>(T& entity, auto tag)
            {
                if constexpr (has_name<T>) allnodes += string(entity.name());
            };
            for_each_node_in_list(list, add_node);
            return allnodes;
        };
    }
}
