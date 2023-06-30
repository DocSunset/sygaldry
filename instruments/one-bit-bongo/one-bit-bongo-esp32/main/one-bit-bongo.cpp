#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <concepts/runtime.hpp>
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
} constinit bongo{};

constexpr auto runtime = Runtime{bongo};

extern "C" void app_main(void)
{
    runtime.init();
    for (;;)
    {
        runtime.tick();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
