\page page-sygbp-test_component sygbp-test_component: Bindings Test Component

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

A component is defined with one of every type of endpoint helper, as an input
and output. It's functionality is to copy its input values to its output
values. This component exists to be used to test bindings. Be warned! This
component should not be wantonly modified, as doing so may break numerous tests!

```cpp
// @#'sygbp-test_component.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace components {
///\addtogroup sygbp
///\{
///\defgroup sygbp-test_component sygbp-test_component: Bindings Test Component
///\{

struct TestComponent : name_<"Test Component 1">
{
    struct inputs_t {
        button<"button in"> button_in;
        toggle<"toggle in"> toggle_in;
        slider<"slider in"> slider_in;
        bng<"bang in"> bang_in;
        text<"text in"> text_in;
        text_message<"text message in"> text_msg_in;
        array<"array in", 3> array_in;
    } inputs;

    struct outputs_t {
        button<"button out"> button_out;
        toggle<"toggle out"> toggle_out;
        slider<"slider out"> slider_out;
        bng<"bang out"> bang_out;
        text<"text out"> text_out;
        text_message<"text message out"> text_msg_out;
        array<"array out", 3> array_out;
    } outputs;

    constexpr void operator()()
    {
        outputs.button_out = inputs.button_in;
        outputs.toggle_out = inputs.toggle_in;
        outputs.slider_out = inputs.slider_in;
        outputs.bang_out = inputs.bang_in;
        outputs.text_out = inputs.text_in;
        outputs.text_msg_out = inputs.text_msg_in;
        outputs.array_out = inputs.array_in;
    }
};

///\}
///\}
} } // namespaces
// @/
```

```cmake
# @#'CMakeLists.txt'
if (SYGALDRY_BUILD_TESTS)

set(lib sygbp-test_component)
add_library(${lib} INTERFACE)
    target_include_directories(${lib} INTERFACE .)
    target_link_libraries(${lib}
            INTERFACE sygah-metadata
            INTERFACE sygah-endpoints
            )
target_link_libraries(sygbp INTERFACE ${lib})

endif()
# @/
```
