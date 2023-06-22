#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <concepts/components.hpp>
#include <components/esp32/button.hpp>
#include <bindings/cli/cli.hpp>
#include <bindings/output_logger.hpp>
#include <bindings/esp32/wifi.hpp>

using namespace sygaldry;

struct OneBitBongo
{
    struct api_t
    {
        bindings::esp32::WiFi<bindings::CstdioLogger> wifi;
        components::esp32::Button<GPIO_NUM_23> button;
    } api;

    bindings::CstdioOutputLogger<decltype(api)> log;
    bindings::CstdioCli<decltype(api)> cli;
} bongo;

static_assert(ComponentContainer<OneBitBongo>);

extern "C" void app_main(void)
{
    init(bongo);
    for (;;)
    {
        clear_output_flags(bongo.api.button);
        bongo.api.button();
        bongo.log(bongo.api);
        bongo.cli(bongo.api);
        clear_input_flags(bongo.api.button);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
