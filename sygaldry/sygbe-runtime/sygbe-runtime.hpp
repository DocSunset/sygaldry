#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sygac-runtime.hpp"
#include "sygsa-two_wire.hpp"
#include "sygbe-spiffs.hpp"
#include "sygbe-wifi.hpp"
#include "sygbp-liblo.hpp"
#include "sygbp-cli.hpp"

namespace sygaldry { namespace sygbe {

/*! \addtogroup sygbe
*/
/// \{

/*! \defgroup sygbe-runtime sygbe-runtime: ESP32 Runtime
*/
/// \{

/*! \brief Runtime wrapper for ESP32 platform

This specialized wrapper for sygaldry::Runtime draws in standard bindings and
components expected to be useful for all instruments based on the ESP32
platform.

*/
template<typename InnerInstrument>
struct ESP32Instrument
{
    struct Instrument {
        struct Components {
            sygsa::TwoWire<21,22,400000> i2c;
            InnerInstrument instrument;
            sygbe::WiFi wifi;
            sygbp::LibloOsc<InnerInstrument> osc;
        };
        sygbe::SpiffsSessionStorage<Components> session_storage;
        Components components;
        sygbp::CstdioCli<Components> cli;
    };

    static inline Instrument instrument{};

    void app_main()
    {
        constexpr auto runtime = Runtime{instrument};
        printf("initializing\n");
        runtime.init();
        // give IDF processes time to finish up init business
        vTaskDelay(pdMS_TO_TICKS(100));
        printf("looping\n");
        while (true)
        {
            runtime.tick();
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
};

/// \}
/// \}

} }
