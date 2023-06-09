#include "unity.h"
#include "gpio.hpp"

extern "C" void app_main(void)
{
    UNITY_BEGIN();
    RUN_TEST(gpio);
    UNITY_END();
}
