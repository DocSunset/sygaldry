\page page-sygin-continulodica_pico Continulodica on Raspberry Pi Pico

Copyright 2024 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

# Implementation

```cpp
// @#'continulodica.cpp'
#include <cmath>
#include <iterator>
#include "sygac-endpoints.hpp"
#include "sygah-metadata.hpp"
#include "sygbr-runtime.hpp"
#include "sygsr-oadc.hpp"
#include "sygsp-continuous-key-scanner.hpp"
#include "sygsr-led_matrix_scanner.hpp"
#include "sygbr-tinyusb_midi_device.hpp"
#include "tusb.h"
#include "bsp/board_api.h"
#include "pico/stdlib.h"

using namespace sygaldry;

template<typename keys_t, typename raw_t>
struct MidiMapping
: name_<"MIDI Mapping">
{
    struct inputs_t {
    } inputs;
    struct outputs_t {
    } outputs;

    void init()
    {
    }

    void main(const keys_t& keys, const raw_t& raws)
    {
        constexpr uint8_t cable = 0;
        constexpr uint8_t channel = 0;
        constexpr uint8_t polyaftertouch = 0xA0;
        constexpr uint8_t status = polyaftertouch | channel;

        static auto last_out = keys;

        // throttle output to a multiple of key scans to reduce MIDI rate
        //static std::size_t count = 0;
        if (not flag_state_of(keys)) return;
        //else if (++count < 20) return; // 20 set empirically so that output rate is ~100Hz
        //count = 0;

        for (uint8_t i = 0; i < keys.size(); ++i)
        {
            //if (keys[i] == last_out[i] && keys[i] <= 0) continue;
            //uint8_t message[3] = {status, i, static_cast<uint8_t>((int(keys[i])>>5)&0x7f)};
            uint8_t message[3] = {status, i, static_cast<uint8_t>(keys[i]*127)};
            tud_midi_stream_write(cable, message, 3);

            uint8_t message2[3] = {polyaftertouch | 1, i, static_cast<uint8_t>((int(raws[i])>>5)&0x7f)};
            tud_midi_stream_write(cable, message2, 3);
            uint8_t message3[3] = {polyaftertouch | 2, i, static_cast<uint8_t>(int(raws[i])&0x7f)};
            tud_midi_stream_write(cable, message3, 3);
        }

        last_out = keys;
    }
};

unsigned int row_pins[] = {5, 4, 3, 2, 1, 0};
unsigned int col_pins[] = {8, 7, 6, 11, 10, 9};

struct Continulodica {
    sygbr::MidiDeviceDriver midi_driver;
    sygsr::OversamplingAdc<sygsr::OADC_CHANNEL_0, 100, 0> adc; // gpio 26
    sygsp::KeyScanner<decltype(adc.outputs.raw), 4096, std::size(row_pins), std::size(col_pins)> scanner;
    sygsr::LedMatrixScanner<decltype(scanner.outputs.leds), std::size(row_pins), std::size(col_pins), row_pins, col_pins> pin_driver;
    MidiMapping<decltype(scanner.outputs.keys), decltype(scanner.outputs.raw)> mapping;
};

sygaldry::sygbr::PicoSDKInstrument<Continulodica> runtime{};
int main(){runtime.main();}
// @/
```

# Build automation

```cmake
# @#'CMakeLists.txt'
cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(sygin-continulodica_pico C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
pico_sdk_init()

add_subdirectory($ENV{SYGALDRY_ROOT} sygbuild)

add_executable(continulodica
    continulodica.cpp
)

#pico_enable_stdio_usb(continulodica ENABLED)

pico_add_extra_outputs(continulodica)

target_link_libraries(continulodica pico_stdlib sygaldry tinyusb_device tinyusb_board)
# @/
```
