#include <string>
#include <catch2/catch_test_macros.hpp>
#include "components/tests/testcomponent.hpp"
#include "bindings/basic_logger/test_logger.hpp"
#include "bindings/output_logger.hpp"

using std::string;

using namespace sygaldry::bindings;

void test_logger(auto& logger, auto& components, string expected_output, auto input_callback)
{
    logger.log.put.ss.str("");
    input_callback();
    components.tc();
    logger(components);
    CHECK(logger.log.put.ss.str() == string(expected_output));
}

struct TestComponents
{
    sygaldry::components::TestComponent tc;
};

TEST_CASE("Output Logger", "[bindings][output_logger]")
{
    auto components = TestComponents{};
    auto& tc = components.tc;
    auto logger = sygaldry::bindings::OutputLogger<TestLogger, decltype(components)>{};

    // updating causes output
    test_logger(logger, components, "/Test_Component_1/button_out 1\n", [&](){
        tc.inputs.button_in = 1;
    });

    // changes cause output
    test_logger(logger, components, "/Test_Component_1/button_out 0\n", [&](){
        tc.inputs.button_in = 0;
    });

    // no output when things stay the same
    test_logger(logger, components, "", [&](){
        tc.inputs.button_in = 0;
    });

    // bangs look different
    test_logger(logger, components, "/Test_Component_1/bang_out\n", [&](){
        tc.inputs.bang_in();
    });

    // bangs always output on bang
    test_logger(logger, components, "/Test_Component_1/bang_out\n", [&](){
        tc.inputs.bang_in();
    });

    // no output when bangs are cleared
    test_logger(logger, components, "", [&](){
        tc.inputs.bang_in = {};
    });
}
