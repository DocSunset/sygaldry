# Test Component 1

A component is defined with one of every type of endpoint helper, as an input
and output. It's functionality is to copy its input values to its output values.
This component can be used to test bindings. Be warned! This component should
not be wantonly modified, as doing so may break numerous tests!

```cpp
// @#'testcomponent.hpp'
#pragma once
#include "helpers/metadata.hpp"
#include "helpers/endpoints.hpp"

namespace sygaldry { namespace components {

struct TestComponent : name_<"Test Component 1">
{
    struct inputs_t {
        button<"button in"> button_in;
        toggle<"toggle in"> toggle_in;
        slider<"slider in"> slider_in;
        bng<"bang in"> bang_in;
        text<"text in"> text_in;
    } inputs;

    struct outputs_t {
        button<"button out"> button_out;
        toggle<"toggle out"> toggle_out;
        slider<"slider out"> slider_out;
        bng<"bang out"> bang_out;
        text<"text out"> text_out;
    } outputs;

    constexpr void operator()()
    {
        outputs.button_out = inputs.button_in;
        outputs.toggle_out = inputs.toggle_in;
        outputs.slider_out = inputs.slider_in;
        outputs.bang_out = inputs.bang_in;
    }
};

} } // namespaces
// @/
```