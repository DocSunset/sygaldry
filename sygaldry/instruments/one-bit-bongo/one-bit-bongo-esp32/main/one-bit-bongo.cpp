
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
#include "sygaldry-concepts-runtime.hpp"
#include "sygaldry-sensors-esp32-button.hpp"
#include "sygaldry-sensors-esp32-adc.hpp"
#include "sygaldry-sensors-arduino-two_wire.hpp"
#include "sygaldry-sensors-esp32-trill.hpp"
//#include "components/icm20948.hpp"
#include "sygaldry-bindings-esp32-spiffs.hpp"
#include "sygaldry-bindings-esp32-wifi.hpp"
#include "sygaldry-bindings-portable-liblo.hpp"
#include "sygaldry-bindings-portable-cli.hpp"
#include "sygaldry-bindings-portable-output_logger.hpp"

using namespace sygaldry;

struct OneBitBongo
{
    struct Instrument
    {
        components::arduino::TwoWire<21,22/*,1000000*/> i2c;
        struct Sensors {
            components::esp32::Button<GPIO_NUM_15> button;
            sensors::esp32::OneshotAdc<33> adc;
            components::arduino::TrillCraft touch;
            //components::arduino::ICM20948 mimu;
        } sensors;
        bindings::esp32::WiFi wifi;
        bindings::LibloOsc<Sensors> osc;
    };

    bindings::esp32::SpiffsSessionStorage<Instrument> session_storage;
    Instrument instrument;
    bindings::CstdioOutputLogger<Instrument> log;
    bindings::CstdioCli<Instrument> cli;
} bongo{};

constexpr auto runtime = Runtime{bongo};

extern "C" void app_main(void)
{
    runtime.init();
    // give IDF processes time to finish up init business
    vTaskDelay(pdMS_TO_TICKS(100));
    while (true)
    {
        runtime.tick();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
