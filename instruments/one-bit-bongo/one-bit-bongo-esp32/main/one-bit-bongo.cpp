#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <components/esp32/gpio/gpio.hpp>

using namespace sygaldry::components::esp32;

extern "C" void app_main(void)
{
    GPIO<GPIO_NUM_23> gpio;
    gpio.init();
    for (;;)
    {
        printf("Hello world %d.\n", sizeof(gpio));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
