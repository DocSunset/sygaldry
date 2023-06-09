#include "unity.h"
#include "gpio.hpp"

extern "C" void app_main(void)
{
    UNITY_BEGIN();
    gpio();
    UNITY_END();
}
