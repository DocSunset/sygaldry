#pragma once

#include "utilities/consteval.hpp"
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

    void operator()()
    {
        if (outputs.debounced_state != inputs.button_state)
        {
            outputs.debounced_state = inputs.button_state;
            outputs.any_edge();
            if (inputs.button_state) outputs.rising_edge();
            else outputs.falling_edge();
        }
    }
};

} }
