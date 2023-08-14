\page bind_output_logger Output Logger

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Following the CLI, the next binding with an optimal balance of usefulness and
simple implementation was one that simply prints a text-based representation of
output endpoint values any time they change.

```cpp
// @#'tests/output_logger/tests.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <string>
#include <catch2/catch_test_macros.hpp>
#include "sygaldry-concepts-components.hpp"
#include "bindings/testcomponent.hpp"
#include "bindings/test_logger.hpp"
#include "bindings/output_logger.hpp"

using std::string;

using namespace sygaldry::bindings;
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
    sygaldry::components::TestComponent tc;
};

TEST_CASE("Output Logger", "[bindings][output_logger]")
{
    auto components = TestComponents{};
    auto& tc = components.tc;
    auto logger = sygaldry::bindings::OutputLogger<TestLogger, decltype(components)>{};

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
// @/
```

```cmake
# @#'tests/output_logger/CMakeLists.txt'
add_executable(output-logger-tests tests.cpp)
target_link_libraries(output-logger-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(output-logger-tests PRIVATE Sygaldry::Bindings)
catch_discover_tests(output-logger-tests)
# @/
```

```cpp
// @#'bindings/output_logger.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#pragma once
#include "bindings/spelling.hpp"
#include "sygaldry-concepts-metadata.hpp"
#include "sygaldry-concepts-components.hpp"
#include "helpers/metadata.hpp"
#include "bindings/osc_string_constants.hpp"
#include "bindings/cstdio_logger.hpp"

namespace sygaldry { namespace bindings {

template<typename Logger, typename Components>
struct OutputLogger : name_<"Output Logger">
{
    struct inputs_t {} inputs;

    [[no_unique_address]] Logger log;

    output_endpoints_t<Components> last_out_list{};

    void external_destinations(Components& components)
    {
        tuple_for_each(last_out_list, [&]<typename T>(T& last_out)
        {
            const T& current_out = find<T>(components);
            if (value_of(current_out) != value_of(last_out))
            {
                if constexpr (Bang<T>)
                {
                    if (value_of(current_out))
                        log.println(osc_path_v<T, Components>);
                    return;
                }
                else
                {
                    last_out = current_out;
                    log.print(osc_path_v<T, Components>);
                    if constexpr (has_value<T>)
                        log.print(" ", value_of(current_out));
                    log.println();
                }
            }
        });
    }
};

template<typename Components> using CstdioOutputLogger = OutputLogger<CstdioLogger, Components>;

} }
// @/
```
