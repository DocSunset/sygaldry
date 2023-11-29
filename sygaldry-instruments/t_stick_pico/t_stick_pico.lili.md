\page page-sygin-t_stick_pico T-Stick for Raspberry Pi Pico

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

# Implementation

```cpp
// @#'tstick.cpp'
#include<cmath>
#include "sygbr-runtime.hpp"
#include "sygsr-button.hpp"
#include "sygsa-trill_craft.hpp"
#include "sygsa-two_wire.hpp"
#include "sygsp-icm20948.hpp"
#include "sygsp-complementary_mimu_fusion.hpp"
#include "sygsa-two_wire_serif.hpp"

using namespace sygaldry;

struct TStick {
    sygsa::TwoWire<0,1,400000> i2c;
    sygsr::Button<26> button;
    sygsa::TrillCraft trill;
    sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                   , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                   > mimu;
    sygsp::ComplementaryMimuFusion<decltype(mimu)> mimu_fusion;
};

sygaldry::sygbr::PicoSDKInstrument<TStick> runtime{};
int main(){runtime.main();}
// @/
```

# Build automation

```cmake
# @#'CMakeLists.txt'
cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(sygin-t_stick_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
pico_sdk_init()

add_subdirectory($ENV{SYGALDRY_ROOT} sygbuild)

add_executable(tstick
    tstick.cpp
)

pico_enable_stdio_usb(tstick 1)

pico_add_extra_outputs(tstick)

target_link_libraries(tstick pico_stdlib sygaldry)
# @/
```
