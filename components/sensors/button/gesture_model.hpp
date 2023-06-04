#pragma once

#include "utilities/consteval.hpp"
#include "components/endpoints/helpers.hpp"

namespace sygaldry
{
namespace components
{

struct ButtonGestureModel
{
    static _consteval auto name() {return "Button Gesture Model";}

    struct inputs_t {
        endpoints::toggle<"button state"> button_state;
    } inputs;

    struct outputs_t {
        // we assume this is persistent across ticks
        endpoints::toggle<"debounced state"> debounced_state;

        endpoints::bng<"any edge"> any_edge;
        endpoints::bng<"rising edge"> rising_edge;
        endpoints::bng<"falling edge"> falling_edge;
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