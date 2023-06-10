#include "unity.h"
#include <components/esp32/tests/gpio.hpp>
#include <components/esp32/tests/button.hpp>

extern "C" void app_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(gpio);
    RUN_TEST(button);
    UNITY_END();
}
