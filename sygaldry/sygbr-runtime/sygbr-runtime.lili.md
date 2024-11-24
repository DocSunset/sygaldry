\page page-sygbr-runtime sygbr-runtime: Raspberry Pi Pico SDK Runtime

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This binding provides a specialization of sygaldry::Runtime for the Raspberry
Pi Pico SDK platform that draws in all available bindings and basic resources
that are expected to be used by all instruments. This can also be read as a
template for a custom runtime for the Pico SDK that may omit some of these
components.

```cpp
// @#'sygbr-runtime.hpp'
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

        stdio_init_all();
        sleep_ms(1000);
        puts("initializing\n");
        runtime.init();

        puts("looping\n");
        while (true)
        {
            runtime.tick();
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
set(lib sygbr-runtime)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygac-runtime
        INTERFACE pico_stdlib
        #INTERFACE sygsr-two_wire
        #INTERFACE sygbr-flash
        #INTERFACE sygbr-wifi
        #INTERFACE sygbp-liblo
        #INTERFACE sygbp-cli
        )
#target_compile_definitions(${lib} INTERFACE
#        PICO_STDIO_DEFAULT_CRLF=0
#        )
# @/
```
