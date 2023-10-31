\page page-sygbe-runtime sygbe-runtime: ESP32 Runtime

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This binding provides a specialization of sygaldry::Runtime for the ESP32
platform that draws in all available bindings and basic resources that are
expected to be used by all instruments. This can also be read as a template
for a custom runtime for the ESP32 that may omit some of these components.

```cpp
// @#'sygbe-runtime.hpp'
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
#include "sygbp-cstdio_cli.hpp"

namespace sygaldry { namespace sygbe {

/*! \addtogroup sygbe sygbe: ESP32 Bindings
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
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygbe-runtime)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygac-runtime
        INTERFACE sygsa-two_wire
        INTERFACE sygbe-spiffs
        INTERFACE sygbe-wifi
        INTERFACE sygbp-liblo
        INTERFACE sygbp-cli
        )
# @/
```