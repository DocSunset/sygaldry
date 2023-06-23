#include <catch2/catch_test_macros.hpp>
#include "concepts/runtime.hpp"
#include "helpers/endpoints.hpp"

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

constinit Runtime<components1_t> runtime{};

TEST_CASE("runtime calls")
{
    runtime.init();
    CHECK(runtime.container.tc1.inputs.in1.value == 42); // init routines are called
    CHECK(runtime.container.tc2.parts.part.inputs.in1.value == 0); // part inits are not called
    runtime.main();
    CHECK(false == (bool)runtime.container.tc1.outputs.bang_out); // out flags are clear after call to main
    CHECK(runtime.container.tc1.outputs.out1.value == 43); // main routines are called
    CHECK(runtime.container.tc2.outputs.out1.value == 44); // throughpoints are propagated
    CHECK(runtime.container.tc2.outputs.out2.value == 44); // plugins are propagated
                                                            // calls proceed in tree order
    CHECK(runtime.container.tc2.parts.part.outputs.out1.value == 0); // part mains are not called
}
