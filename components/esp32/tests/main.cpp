#include "unity.h"
#include "gpio.hpp"
#include "button.hpp"

extern "C" void app_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(gpio);
    RUN_TEST(button);
    UNITY_END();
}
