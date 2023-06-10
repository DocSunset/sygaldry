#pragma once

#include <components/sensors/button.hpp>
#include <components/esp32/gpio.hpp>

namespace sygaldry { namespace components { namespace esp32 {

enum class ButtonActive {High, Low};

template<gpio_num_t pin_number, ButtonActive active_level = ButtonActive::Low>
struct Button
: name_<"Button">
, author_<"Travis J. West">
, copyright_<"Travis J. West (C) 2023">
, description_<"A single button attached to a GPIO">
{
    ButtonGestureModel::inputs_t inputs;
    ButtonGestureModel::outputs_t outputs;
    struct parts_t { GPIO<pin_number> gpio; } parts;

    void init()
    {
        parts.gpio.init();
        parts.gpio.inputs.input_mode(parts.gpio);
        if constexpr (active_level == ButtonActive::Low) parts.gpio.inputs.enable_pullup(parts.gpio);
        else parts.gpio.inputs.enable_pulldown(parts.gpio);
    }

    void operator()()
    {
        inputs.button_state = (char)parts.gpio.outputs.level();
        ButtonGestureModel::main(inputs, outputs);
    }
};
    
} } }
