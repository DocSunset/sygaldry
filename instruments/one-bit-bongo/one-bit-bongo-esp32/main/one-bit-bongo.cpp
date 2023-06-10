#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <bindings/cli/cli.hpp>
#include <components/esp32/gpio.hpp>

using namespace sygaldry::components;
//using namespace sygaldry::bindings;

struct components_t
{
    esp32::GPIO<GPIO_NUM_23> gpio;
} components;

//CstdioCli<components> cli;

extern "C" void app_main(void)
{
    components.gpio.init();
    //cli.init();
    for (;;)
    {
        //cli();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
