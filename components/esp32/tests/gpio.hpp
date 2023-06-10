#pragma once

#include <components/esp32/gpio.hpp>

void gpio()
{
    using pin = sygaldry::components::esp32::GPIO<GPIO_NUM_23>;
    pin::init();
    pin::input_mode();
    pin::enable_pullup();
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, pin::level(), "input mode pin with pullup should read high level");
    pin::disable_pullup();
    pin::enable_pulldown();
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pin::level(), "input mode pin with pulldown should read low level");
}
