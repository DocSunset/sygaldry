#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <concepts/components.hpp>
#include <bindings/cli/cli.hpp>
#include <bindings/output_logger.hpp>
#include <components/esp32/button.hpp>

using namespace sygaldry::components;
using namespace sygaldry::bindings;

struct components_t
{
    esp32::Button<GPIO_NUM_23> button;
} components;

CstdioCli<decltype(components)> cli;
CstdioOutputLogger<decltype(components)> log;

extern "C" void app_main(void)
{
    components.button.init();
    cli.init();
    for (;;)
    {
        components.button();
        log(components);
        cli(components);
        clear_flags(components.button);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
