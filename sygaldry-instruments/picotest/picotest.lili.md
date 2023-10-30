\page page-sygin-picotest Raspberry Pi Pico Test

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

# Implementation

```cpp
// @#'test.cpp'
#include<cmath>
#include "sygbr-runtime.hpp"
#include "sygbp-test_component.hpp"
#include "sygsr-button.hpp"

struct Test {
    sygaldry::sygbp::TestComponent tc;
    sygaldry::sygsr::Button<15, sygaldry::sygsp::ButtonActive::High> button;
};

sygaldry::sygbr::PicoSDKInstrument<Test> runtime{};
int main(){runtime.main();}
// @/
```

# Build automation

```cmake
# @#'CMakeLists.txt'
cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
pico_sdk_init()

add_subdirectory($ENV{SYGALDRY_ROOT} sygbuild)

add_executable(test
    test.cpp
)

pico_enable_stdio_usb(test 1)

pico_add_extra_outputs(test)

target_link_libraries(test pico_stdlib sygaldry)
# @/
```
