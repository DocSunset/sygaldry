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
