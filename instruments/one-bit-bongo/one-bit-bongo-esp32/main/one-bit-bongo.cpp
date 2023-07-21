
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Trill.h>
#include <concepts/runtime.hpp>
#include <components/esp32/button.hpp>
#include <bindings/esp32/spiffs.hpp>
#include <bindings/esp32/wifi.hpp>
#include <bindings/liblo.hpp>
#include <bindings/cli/cli.hpp>
#include <bindings/output_logger.hpp>

using namespace sygaldry;

struct OneBitBongo
{
    struct Instrument
    {
        bindings::esp32::WiFi<bindings::CstdioLogger> wifi;
        components::esp32::Button<GPIO_NUM_23> button;
        bindings::LibloOsc<decltype(button)> osc;
    };

    bindings::esp32::SpiffsSessionStorage<Instrument> session_storage;
    Instrument instrument;
    bindings::CstdioOutputLogger<Instrument> log;
    bindings::CstdioCli<Instrument> cli;
} bongo{};

constexpr auto runtime = Runtime{bongo};

extern "C" void app_main(void)
{
    Trill trill{};
    runtime.init();
    for (;;)
    {
        runtime.tick();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
