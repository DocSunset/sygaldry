\page page-sygsp-button sygsp-button: Button Gesture Model

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

Although a button is a simple digital sensor with only two states. However,
when the temporal evolution of those states is considered, a variety of
meaningfully distinct gestures can be performed even with a single button.

It is useful in many circumstances to detect button state transitions, such as
the rising edge when the button state is changed to `1`, and the falling edge
when the button is released. Each transition may be used to trigger different
behaviors. This mapping is indeed so basic and essential that it is necessary
to implement many other typical button gesture models.

It may be useful in to differentiate when the button is pressed down and held,
compared to when it is pressed and then quickly released. This could be used,
for example, to enable a function momentarily while holding the button, or to
persistently toggle the function on or off with a tap. In a live looper this
could be used with recording or overdub functions, for example.

It may be useful to differentiate between a single tap and two or more taps in
quick succession. One tap might trigger a certain event, while two taps trigger
a different event, and three another. One button is transformed into an arbitrary
number of discrete triggers, limited only by how quickly you can tap.

For now, the following gesture model only detects state transitions. Other
useful mappings are left for future work.

Note that this component is unnamed. It is expected that a platform-specific
component will inherit its inputs and outputs and call on its main subroutine
to implement a button for specific hardware, and that said platform-specific
component will implement a name.

```cpp
// @#'sygsp-button.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-consteval.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace sygsp {
///\addtogroup sygsp sygsp: Portable Sensors
///\{
///\defgroup sygsp-button sygsp-button: Button Gesture Model
///\{

/// Enumeration for button activity to button logical state mapping
/*!

When a button is active high, it implies that the logical voltage state of the
button is high (1, VCC) when the button is "activated", e.g. when a push-button
is pushed, and low (0, GND) when the button is not activated. Similarly, a
button that is active low reads logic low (0, GND) when activated and logic
high (1, VCC) when not activated.

Button implementations are expected to set the `button_state` toggle to 1 when
the button is active, as in e.g. `inputs.button_state = read_gpio() ==
active_level` where `read_gpio()` reads the GPIO the button is attached to and
`active_level` is a `ButtonActive` value.

*/
enum class ButtonActive : char {Low = 0, High = 1};


/// Component modelling gestures performed with a single bi-state push button.
struct ButtonGestureModel
{
    struct inputs_t {
        toggle<"button state", "1 indicates button active, 0 inactive"> button_state;
    } inputs;

    struct outputs_t {
        // we assume this is persistent across ticks
        toggle<"debounced state", "updates only when the button state has changed since the last tick"> debounced_state;

        bng<"any edge", "bangs on any change to debounced state"> any_edge;
        bng<"rising edge", "bangs when the button is activated"> rising_edge;
        bng<"falling edge", "bangs when the button is deactivated"> falling_edge;
    } outputs;

    void operator()();
};

///\}
///\}
} }
// @/

// @#'sygsp-button.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygsp-button.hpp"

namespace sygaldry { namespace sygsp {

void ButtonGestureModel::operator()()
{
    if (outputs.debounced_state != inputs.button_state)
    {
        outputs.debounced_state = inputs.button_state;
        outputs.any_edge();
        if (inputs.button_state) outputs.rising_edge();
        else outputs.falling_edge();
    }
}

} }
// @/
```

TODO: write tests for this component

```cmake
# @#'CMakeLists.txt'
set(lib sygsp-button)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib}
        PUBLIC sygah-consteval
        PUBLIC sygah-endpoints
        )

#if(SYGALDRY_BUILD_AVENDISH)
#avnd_make_pd(TARGET ${lib}
#        MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${lib}.hpp"
#        MAIN_CLASS sygaldry::sygsp::ButtonGestureModel
#        C_NAME sygsp_button
#        )
#target_link_libraries(${lib}_pd
#        PRIVATE ${lib}
#        PRIVATE sygah-consteval
#        PRIVATE sygah-endpoints
#        )
#endif()
# @/
```
