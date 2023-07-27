
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
#include <components/esp32/i2c.hpp>
#include <components/trill_craft.hpp>
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
        components::esp32::I2C<21,22/*,1000000*/> i2c;
        struct Sensors {
            components::esp32::Button<GPIO_NUM_15> button;
            components::TrillCraft trill;
        } sensors;
        bindings::esp32::WiFi<bindings::CstdioLogger> wifi;
        bindings::LibloOsc<Sensors> osc;
    };

    bindings::esp32::SpiffsSessionStorage<Instrument> session_storage;
    Instrument instrument;
    //bindings::CstdioOutputLogger<Instrument> log;
    bindings::CstdioCli<Instrument> cli;
} bongo{};

constexpr auto runtime = Runtime{bongo};

extern "C" void app_main(void)
{
    runtime.init();
    // give IDF processes time to finish up init business
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    while (true)
    {
        runtime.tick();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}
