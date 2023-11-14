\page page-sygin-t_stick T-Stick

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

## Implementation

```cpp
// @#'main/button.cpp'
#include "sygse-button.hpp"
#include "syghe-pins.hpp"

using namespace sygaldry;
template struct sygse::Button<GPIO_NUM_15>;
// @/
```

```cpp
// @#'main/adc.cpp'
#include "sygse-adc.hpp"
#include "syghe-pins.hpp"

using namespace sygaldry;
template struct sygse::OneshotAdc<syghe::ADC1_CHANNEL_5>;
// @/
```

```cpp
// @#'main/mimu.cpp'
#include "sygsp-icm20948.hpp"
#include "sygsa-two_wire_serif.hpp"
#include "sygsp-complementary_mimu_fusion.hpp"

using namespace sygaldry;
template struct sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                               , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                               >;
template struct sygsp::ComplementaryMimuFusion<sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                               , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                               >>;
// @/
```

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
#include "sygse-trill.hpp"
#include "sygse-max17055.hpp"
#include "sygsp-restart-agent.hpp"
#include "sygsp-icm20948.hpp"
#include "sygsa-two_wire_serif.hpp"
#include "sygsp-complementary_mimu_fusion.hpp"
#include "sygbe-runtime.hpp"

using namespace sygaldry;

extern template struct sygse::Button<GPIO_NUM_15>;
extern template struct sygse::OneshotAdc<syghe::ADC1_CHANNEL_5>;
extern template struct sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                               , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                               >;
extern template struct sygsp::ComplementaryMimuFusion<sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                               , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                               >>;

struct TStick
{
    sygse::Button<GPIO_NUM_15> button;
    sygse::OneshotAdc<syghe::ADC1_CHANNEL_5> adc;
    sygsa::TrillCraft touch;
    sygsa::MAX17055<2600, 10, 60000> fuelgauge;
    sygsp::RestartAgent<decltype(MAX17055), "MAX17055"> fuelgauge_agent;
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
        "button.cpp"
        "mimu.cpp"
        "adc.cpp"
        )
add_subdirectory(../../../ sygbuild) # add sygaldry as a subdirectory
target_compile_options(${COMPONENT_LIB} PRIVATE
        "-Wfatal-errors"
        "-Wno-error=unused-but-set-parameter"
        "-ftemplate-backtrace-limit=0"
        )
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
