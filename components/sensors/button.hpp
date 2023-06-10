#pragma once

#include "utilities/consteval.hpp"
#include "helpers/endpoints.hpp"

namespace sygaldry { namespace components {

using namespace sygaldry::helpers;

struct ButtonGestureModel
{
    static _consteval auto name() {return "Button Gesture Model";}

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

    static void main(const inputs_t& in, outputs_t& out)
    {
        if (out.debounced_state != in.button_state)
        {
            out.debounced_state = in.button_state;
            out.any_edge();
            if (in.button_state) out.rising_edge();
            else out.falling_edge();
        }
    }

    void operator()()
    {
        main(inputs, outputs);
    }
};

} }
