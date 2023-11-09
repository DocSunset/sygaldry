/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <string>
#include <catch2/catch_test_macros.hpp>
#include "sygac-components.hpp"
#include "sygbp-test_component.hpp"
#include "sygup-test_logger.hpp"
#include "sygbp-output_logger.hpp"

using std::string;

using namespace sygaldry::sygbp;
using namespace sygaldry;

void test_logger(auto& logger, auto& components, string expected_output, auto input_callback)
{
    logger.log.put.ss.str("");
    input_callback();
    components.tc();
    logger.external_destinations(components);
    CHECK(logger.log.put.ss.str() == string(expected_output));
}

struct TestComponents
{
    sygaldry::sygbp::TestComponent tc;
};

TEST_CASE("sygaldry Output Logger", "[bindings][output_logger]")
{
    auto components = TestComponents{};
    auto& tc = components.tc;
    auto logger = sygaldry::sygbp::OutputLogger<sygup::TestLogger, decltype(components)>{};

    static_assert(Component<decltype(logger)>);

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
