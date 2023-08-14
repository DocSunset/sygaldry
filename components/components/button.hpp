#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygaldry-utilities-consteval.hpp"
#include "helpers/endpoints.hpp"

namespace sygaldry { namespace components {

struct ButtonGestureModel
{
    struct inputs_t {
        toggle<"button state"> button_state;
    } inputs;

    struct outputs_t {
        // we assume this is persistent across ticks
        toggle<"debounced state"> debounced_state;

        bng<"any edge"> any_edge;
        bng<"rising edge"> rising_edge;
        bng<"falling edge"> falling_edge;
    } outputs;

    void operator()();
};

} }
