\page page-sygin-t_stick_esp32s3 T-Stick on ESP32S3

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

## Implementation

```cpp
// @#'main/t_stick.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygse-button.hpp"
#include "sygse-adc.hpp"
#include "sygsa-two_wire.hpp"
#include "sygse-trill.hpp"
#include "sygsp-icm20948.hpp"
#include "sygsa-two_wire_serif.hpp"
#include "sygsp-complementary_mimu_fusion.hpp"
#include "sygbe-runtime.hpp"
#include "sygup-debug_printer.hpp"
#include "sygup-cstdio_logger.hpp"

using namespace sygaldry;

struct TStick
{
    sygse::Button<GPIO_NUM_21> button;
    sygse::OneshotAdc<syghe::ADC1_CHANNEL_5> adc;
    sygsa::TrillCraft touch;
    sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                   , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                   > mimu;
    sygsp::ComplementaryMimuFusion<decltype(mimu)> mimu_fusion;
};

sygbe::ESP32Instrument<TStick> tstick{};
extern "C" void app_main(void) { tstick.app_main(); }
// @/
```

## Build Boilerplate

The `idf.py init` generated boilerplate `CMakeLists.txt` for an `esp-idf` project:

```cmake
# @#'CMakeLists.txt'
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(t-stick)
# @/
```

```cmake
# @#'main/CMakeLists.txt'
idf_component_register(SRCS "t_stick.cpp"
        )
add_subdirectory(../../../ sygbuild) # add sygaldry as a subdirectory
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wfatal-errors" "-ftemplate-backtrace-limit=0")
target_link_libraries(${COMPONENT_LIB} PRIVATE sygaldry)
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