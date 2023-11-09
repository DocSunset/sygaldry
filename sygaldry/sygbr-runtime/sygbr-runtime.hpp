#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "sygac-runtime.hpp"
#include "sygbr-cli.hpp"

namespace sygaldry { namespace sygbr {

/*! \addtogroup sygbr sygbr: Raspberry Pi Pico SDK Bindings
*/
/// \{

/*! \defgroup sygbr-runtime sygbr-runtime: Raspbery Pi Pico SDK Runtime
*/
/// \{

/*! \brief Runtime wrapper for Raspberry Pi Pico SDK platform

This specialized wrapper for sygaldry::Runtime draws in standard bindings and
components expected to be useful for all instruments based on the Raspberry Pi
Pico SDK platform.

*/
template<typename InnerInstrument>
struct PicoSDKInstrument
{
    struct Instrument {
        struct Components {
            //sygsa::TwoWire<21,22,400000> i2c;
            InnerInstrument instrument;
            //sygbr::WiFi wifi;
            //sygbp::LibloOsc<InnerInstrument> osc;
        };
        //sygbr::FlashSessionStorage<Components> session_storage;
        Components components;
        PicoCli<Components> cli;
    };

    static inline Instrument instrument{};

    void main()
    {
        constexpr auto runtime = Runtime{instrument};
        const uint LED_PIN = 25;

        stdio_init_all();
        puts("initializing\n");
        runtime.init();

            gpio_init(LED_PIN);
            gpio_set_dir(LED_PIN, GPIO_OUT);

        puts("looping\n");
        while (true)
        {
            runtime.tick();

                gpio_put(LED_PIN, 0);
                sleep_ms(250);
                gpio_put(LED_PIN, 1);
                sleep_ms(250);
        }
    }
};

/// \}
/// \}

} }
