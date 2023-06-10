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

    using gpio = GPIO<pin_number>;

    void init()
    {
        gpio::init();
        gpio::input_mode();
        if constexpr (active_level == ButtonActive::Low) gpio::enable_pullup();
        else gpio::enable_pulldown();
    }

    void operator()()
    {
        inputs.button_state = (char)gpio::level();
        ButtonGestureModel::main(inputs, outputs);
    }
};
    
} } }
