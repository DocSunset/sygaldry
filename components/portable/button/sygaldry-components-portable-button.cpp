/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygaldry-components-portable-button.hpp"

namespace sygaldry { namespace components { namespace portable {

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

} } }
