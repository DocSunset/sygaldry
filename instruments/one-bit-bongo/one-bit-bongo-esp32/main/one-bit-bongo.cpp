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

CstdioOutputLogger<decltype(components)> log;
CstdioCli<decltype(components)> cli;

extern "C" void app_main(void)
{
    components.button.init();
    cli.init();
    for (;;)
    {
        clear_output_flags(components.button);
        components.button();
        log(components);
        cli(components);
        clear_input_flags(components.button);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
