\page inst_bongo_esp32 ESP32 One Bit Bongo

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

Originally intended as a minimum viable DMI, this instrument
is currently used to test all of the ESP32-compatible components.

[TOC]

## Implementation

```cpp
// @#'main/one-bit-bongo.cpp'

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
#include <concepts/runtime.hpp>
#include <components/esp32/button.hpp>
#include <components/esp32/i2c.hpp>
#include <components/trill_craft.hpp>
#include <components/icm20948.hpp>
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
            components::TrillCraft touch;
            components::ICM20948 mimu;
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
// @/
```

## Build Boilerplate

The `idf.py init` generated boilerplate `CMakeLists.txt` for an `esp-idf` project:

```cmake
# @#'CMakeLists.txt'
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(EXTRA_COMPONENT_DIRS ${SYGALDRY_ROOT}/components/esp32 ${SYGALDRY_ROOT}/bindings/esp32)
project(one-bit-bongo)
# @/
```

Note that we add the ESP32 components directories as `EXTRA_COMPONENT_DIRS` so
that IDF components (such as the `libmapper-arduino` component) will be picked
up by the build system.

We add `sygaldry` as an include directory, as well as its main header-only
dependencies. This should arguably be handled by making `sygaldry` itself
into an IDF component; this is left as future work.

Note that, for the following

```cmake
# @#'main/CMakeLists.txt'
idf_component_register(SRCS "one-bit-bongo.cpp"
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/pfr/include
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/mp11/include
        INCLUDE_DIRS "." ${SYGALDRY_ROOT}
        )
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wfatal-errors")
# @/
```

## Partition Table

In order to use SPIFFS for session data storage, we are required
to provide a custom partition table that declares a data partition with
`spiffs` subtype where the SPIFFS will be located.

```csv
# @#'partitions.csv'
# name,   type, subtype,   offset,     size,   flags
nvs,      data, nvs,       0x9000,   0x4000,
phy_init, data, phy,       0xf000,   0x1000,
main,     app,  factory,  0x10000, 0x290000,
storage,  data, spiffs,  0x300000,       1M,
# @/
```

The `sdkconfig` is then directed to use this partition table by setting
the following two lines:

```
CONFIG_PARTITION_TABLE_CUSTOM=y
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions.csv"
```

This is done manually to avoid having to reproduce the entire `sdkconfig` here.
