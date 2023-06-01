#pragma once

#include "components/ports/ports.hpp"

namespace sygaldry
{
namespace components
{

struct ButtonGestureModel
{
    static consteval auto name() {return "Button Gesture Model";}

    struct inputs_t {
        ports::toggle<"button state"> button_state;
    } inputs;

    struct outputs_t {
        // we assume this is persistent across ticks
        ports::toggle<"debounced state"> debounced_state;

        ports::bng<"any edge"> any_edge;
        ports::bng<"rising edge"> rising_edge;
        ports::bng<"falling edge"> falling_edge;
    } outputs;

    void operator()(const inputs_t& in, outputs_t& out)
    {
        if (out.debounced_state != in.button_state)
        {
            out.debounced_state = inputs.button_state;
            out.any_edge();
            if (in.button_state) out.rising_edge();
            else out.falling_edge();
        }
    }

    void operator()()
    {
        operator()(inputs, outputs);
    }
};

}
}
