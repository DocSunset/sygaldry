/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "sygaldry-concepts-runtime.hpp"
#include "sygaldry-helpers-endpoints.hpp"

using namespace sygaldry;

template<string_literal str>
struct testcomponent1_t : name_<str>
{
    struct inputs_t {
        struct in1_t {
            int value;
        } in1;
    } inputs;

    struct outputs_t {
        struct out1_t {
            int value;
        } out1;
        bng<"bang out"> bang_out;
    } outputs;

    void init() { inputs.in1.value = 42; }
    void main()
    {
        outputs.out1.value = inputs.in1.value + 1;
        CHECK(false == (bool)outputs.bang_out); // output flags are clear on entry to main
        outputs.bang_out();
        CHECK(true == (bool)outputs.bang_out);
    }
};

struct testcomponent2_t : name_<"tc2">
{
    struct outputs_t {
        struct out1_t {
            int value;
        } out1;
        struct out2_t {
            int value;
        } out2;
    } outputs;

    struct parts_t {
        testcomponent1_t<"part"> part;
    } parts;

    void init() {};

    void main(const testcomponent1_t<"tc1">::outputs_t& sources, testcomponent1_t<"tc1">& plugin)
    {
        CHECK(true == (bool)plugin.outputs.bang_out); // flags propagate during main tick
        outputs.out1.value = sources.out1.value + 1;
        outputs.out2.value = plugin.outputs.out1.value + 1;
    }
};

struct components1_t
{
    testcomponent1_t<"tc1"> tc1;
    testcomponent2_t tc2;
};
constinit components1_t components{};
constexpr auto runtime = Runtime{components};
TEST_CASE("runtime calls")
{
    runtime.init();
    CHECK(runtime.container.tc1.inputs.in1.value == 42); // init routines are called
    CHECK(runtime.container.tc2.parts.part.inputs.in1.value == 0); // part inits are not called
    runtime.tick();
    CHECK(false == (bool)runtime.container.tc1.outputs.bang_out); // out flags are clear after call to main
    CHECK(runtime.container.tc1.outputs.out1.value == 43); // main routines are called
    CHECK(runtime.container.tc2.outputs.out1.value == 44); // throughpoints are propagated
    CHECK(runtime.container.tc2.outputs.out2.value == 44); // plugins are propagated
                                                            // calls proceed in tree order
    CHECK(runtime.container.tc2.parts.part.outputs.out1.value == 0); // part mains are not called
}
components1_t constinit main_runtime_components{};
component_runtime<testcomponent2_t, components1_t> constinit main_component_runtime{main_runtime_components.tc2, main_runtime_components};
TEST_CASE("component runtime main")
{
    main_runtime_components.tc1.outputs.out1.value = 0;
    main_runtime_components.tc1.outputs.bang_out();
    main_component_runtime.main();
    CHECK(main_runtime_components.tc2.outputs.out1.value == 1);
    CHECK(main_runtime_components.tc2.outputs.out2.value == 1);
}
components1_t constinit init_runtime_components{};
component_runtime<testcomponent1_t<"tc1">, components1_t> constinit init_component_runtime{init_runtime_components.tc1, init_runtime_components};
TEST_CASE("component runtime init")
{
    init_runtime_components.tc1.inputs.in1.value = 0;
    init_component_runtime.init();
    CHECK(init_runtime_components.tc1.inputs.in1.value == 42);
}
components1_t constinit runtime_tuple_components{};
constexpr auto runtime_tuple = component_to_runtime_tuple(runtime_tuple_components);
TEST_CASE("runtime tuple")
{
    std::apply([](auto& ... runtime) {(runtime.init(), ...);}, runtime_tuple);
    CHECK(runtime_tuple_components.tc1.inputs.in1.value == 42); // init routines are called
    std::apply([](auto& ... runtime) {(runtime.main(), ...);}, runtime_tuple);
    CHECK(runtime_tuple_components.tc1.outputs.out1.value == 43); // main routines are called
}
