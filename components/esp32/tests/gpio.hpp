#pragma once

#include <components/esp32/gpio.hpp>

void gpio()
{
    sygaldry::components::esp32::GPIO<GPIO_NUM_23> pin;
    pin.init();
    pin.inputs.input_mode(pin);
    pin.inputs.enable_pullup(pin);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, pin.outputs.level(), "input mode pin with pullup should read high level");
    pin.inputs.disable_pullup(pin);
    pin.inputs.enable_pulldown(pin);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pin.outputs.level(), "input mode pin with pulldown should read low level");
}
