\page page-sygaldry-docs-implementation Implementation Documentation

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

The source files are annotated with literate prose meant to facilitate the
reader's understanding of the code. For more detailed documentation of the
library than Doxygen comments can provide, this is the authoritative source.

## Instruments

- \subpage inst_bongo_esp32 A testing instrument for the ESP32 platform
- \subpage inst_t_stick A non-functional sketch of the T-Stick implementation

## Sensors

### Portable

- \subpage page-sygaldry-sensors-portable-arduino_hack
- \subpage page-sygaldry-sensors-portable-button
- \subpage page-sygaldry-sensors-portable-debug_printer

### Arduino

- \subpage page-sygaldry-sensors-arduino-icm20948
- \subpage page-sygaldry-sensors-arduino-trill_craft
- \subpage page-sygaldry-sensors-arduino-two_wire

### ESP32

- \subpage page-sygaldry-sensors-esp32-adc
- \subpage page-sygaldry-sensors-esp32-arduino_hack
- \subpage page-sygaldry-sensors-esp32-button
- \subpage page-sygaldry-sensors-esp32-gpio
- \subpage page-sygaldry-sensors-esp32-icm20948
- \subpage page-sygaldry-sensors-esp32-trill

## Bindings

- \subpage readme_bindings
- \subpage bind_cmake

### Platform Independent

- \subpage bind_cli
- \subpage bind_basic_logger
- \subpage bind_basic_reader
- \subpage bind_spelling
- \subpage bind_osc_string_constants
- \subpage bind_osc_match_pattern
- \subpage bind_liblo
- \subpage bind_rapidjson
- \subpage bind_session_data
- \subpage bind_output_logger
- \subpage bind_demo

### ESP32

- \subpage bind_libmapper_arduino
- \subpage bind_spiffs
- \subpage bind_wifi
- \subpage bind_rapidjson

## Helpers

\subpage readme_helpers TODO

- \subpage helpers_cmake
- \subpage helpers_metadata
- \subpage helpers_endpoints
- \subpage helpers_mimu

## Concepts

- \subpage concepts_cmake
- \subpage concepts_runtime
- \subpage concepts_components
- \subpage concepts_endpoints
- \subpage concepts_metadata
- \subpage concepts_mimu
- \subpage concepts_functions

## Other

- \subpage build_system
- \subpage consteval_and_clang
- \subpage sygaldry_idf_component
