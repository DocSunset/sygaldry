#pragma once

struct ButtonGestureModel
{
    static consteval auto name() { return "Button Gesture Model"; }

    struct inputs_t {
        struct button_state_t {
            static consteval auto name() { return "button state"; }
            bool value;
        } button_state;
    } inputs;

    struct outputs_t {
        struct rising_edge_t {
            static consteval auto name() { return "rising edge"; }
            bool value;
        } rising_edge;

        struct falling_edge_t {
            static consteval auto name() { return "falling edge"; }
            bool value;
        } falling_edge;

        struct any_edge_t {
            static consteval auto name() { return "any edge"; }
            bool value;
        } any_edge;

        struct debounced_state_t {
            static consteval auto name() { return "debounced state"; }
            bool value;
        } debounced_state;
    } outputs;

    void operator()()
    {
        if (outputs.debounced_state.value != inputs.button_state.value)
        {
            outputs.debounced_state.value = inputs.button_state.value;
            outputs.rising_edge.value = inputs.button_state.value;
            outputs.falling_edge.value = not inputs.button_state.value;
            outputs.any_edge.value = true;
        }
        else
        {
            outputs.rising_edge.value = outputs.falling_edge.value
                                      = outputs.any_edge.value = false;
        }
    }
};
