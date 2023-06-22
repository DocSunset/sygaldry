# Button Gesture Model

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

```cpp
// @#'button.hpp'
#pragma once

#include "utilities/consteval.hpp"
#include "helpers/endpoints.hpp"

namespace sygaldry { namespace components {

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
// @/
```

TODO: write tests for this component
