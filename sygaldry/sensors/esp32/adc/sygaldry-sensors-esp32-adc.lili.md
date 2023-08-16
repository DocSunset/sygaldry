\page page-sygaldry-sensors-esp32-adc ESP32 ADC Component

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

The ADC component provides a very basic wrapper around the one-shot ADC read
API provided by the ESP-IDF; it is essentially a wrapper around the
[`oneshot_read_main.c` example](https://github.com/espressif/esp-idf/blob/v5.1-rc1/examples/peripherals/adc/oneshot_read/main/oneshot_read_main.c)
provided with the IDF documentation.

[TOC]

# Implementation

## Init

## Main

# Summary

```cpp
// @#'sygaldry-sensors-esp32-adc.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygaldry-helpers-endpoints.hpp"
#include "sygaldry-helpers-metadata.hpp"

namespace sygaldry { namespace component { namespace esp32

template<int gpio_num>
struct OneshotAdc
: name_<"ESP32 Oneshot ADC">
, author_<"Travis J. West"
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
{
    struct outputs_t {
        slider<"raw", "raw binary representation of the analog voltage measured by the ADC"
        , int, 0, 4096, 0 // esp32 TRM V5 section 29.3 says 12 bit max resolution
        > raw;

        slider<"voltage", "the estimated analog voltage measured by the ADC"
        , float, 0.0f, 3.3f, 0.0f
        > voltage;
    } outputs;

    void * pimpl;
    void init();
    void main();
};

} } }
// @/

// @#'sygaldry-sensors-esp32-adc.cpp'
#include "esp_adc/adc_oneshot.h"
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygaldry-sensors-esp32-adc)

add_library(${lib} STATIC)
target_sources(${lib}
        PRIVATE ${lib}.cpp
        )
target_include_directories(${lib}
        PUBLIC .
        )
target_link_libraries(${lib}
        PUBLIC sygaldry-helpers-endpoints
        PUBLIC sygaldry-helpers-metadata
        )

target_link_libraries(sygaldry-sensors-esp32 INTERFACE ${lib})
# @/
```
