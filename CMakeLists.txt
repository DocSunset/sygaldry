# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.24)
project(Sygaldry)

set(SYGALDRY_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

if (NOT ESP_PLATFORM)
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2 # or a later release
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
set(SYGALDRY_BUILD_TESTS 1)
endif()

add_subdirectory(dependencies/pfr)
add_subdirectory(dependencies/mp11)

add_library(sygaldry INTERFACE)

add_library(sygaldry-utilities INTERFACE)
    add_subdirectory(sygaldry/utilities/consteval)
target_link_libraries(sygaldry INTERFACE sygaldry-utilities)

add_library(sygaldry-concepts INTERFACE)
    add_subdirectory(sygaldry/concepts/components)
    add_subdirectory(sygaldry/concepts/endpoints)
    add_subdirectory(sygaldry/concepts/functions)
    add_subdirectory(sygaldry/concepts/metadata)
    add_subdirectory(sygaldry/concepts/mimu)
    add_subdirectory(sygaldry/concepts/runtime)
target_link_libraries(sygaldry INTERFACE sygaldry-concepts)

add_library(sygaldry-helpers INTERFACE)
    add_subdirectory(sygaldry/helpers/endpoints)
    add_subdirectory(sygaldry/helpers/metadata)
    add_subdirectory(sygaldry/helpers/mimu)
target_link_libraries(sygaldry INTERFACE sygaldry-helpers)

add_library(sygaldry-components INTERFACE)

    add_library(sygaldry-sensors INTERFACE)

        add_library(sygaldry-sensors-portable INTERFACE)
            # arduino-hack, alphabetically missing here, is added only where required by a platform
            add_subdirectory(sygaldry/sensors/portable/button)
            add_subdirectory(sygaldry/sensors/portable/array_order_mapping)
        target_link_libraries(sygaldry-sensors INTERFACE sygaldry-sensors-portable)

        if (ESP_PLATFORM)
            add_library(sygaldry-sensors-esp32 INTERFACE)
            add_library(sygaldry-sensors-arduino INTERFACE)
                add_subdirectory(sygaldry/sensors/portable/arduino-hack)
                add_subdirectory(sygaldry/sensors/esp32/arduino-hack)
                add_subdirectory(sygaldry/sensors/arduino/two_wire)
                add_subdirectory(sygaldry/sensors/arduino/trill_craft)
                add_subdirectory(sygaldry/sensors/esp32/adc)
                add_subdirectory(sygaldry/sensors/esp32/button)
                add_subdirectory(sygaldry/sensors/esp32/gpio)
                    #add_subdirectory(sygaldry/sensors/esp32/icm20948)
                add_subdirectory(sygaldry/sensors/esp32/trill)
            target_link_libraries(sygaldry-sensors INTERFACE sygaldry-sensors-esp32)
            target_link_libraries(sygaldry-sensors INTERFACE sygaldry-sensors-arduino)
        endif()

    target_link_libraries(sygaldry-components INTERFACE sygaldry-sensors)

    add_library(sygaldry-bindings INTERFACE)

        add_library(sygaldry-bindings-portable INTERFACE)
            add_subdirectory(sygaldry/bindings/portable/basic_reader/cstdio_reader)
            add_subdirectory(sygaldry/bindings/portable/cli)
            add_subdirectory(sygaldry/bindings/portable/osc/liblo)
            add_subdirectory(sygaldry/bindings/portable/osc/match_pattern)
            add_subdirectory(sygaldry/bindings/portable/osc/string_constants)
            add_subdirectory(sygaldry/bindings/portable/output_logger)
            add_subdirectory(sygaldry/bindings/portable/rapidjson)
            add_subdirectory(sygaldry/bindings/portable/session_data)
            add_subdirectory(sygaldry/bindings/portable/spelling)
            if (SYGALDRY_BUILD_TESTS)
                add_subdirectory(sygaldry/bindings/portable/basic_reader/test_reader)
                add_subdirectory(sygaldry/bindings/portable/test_component)
            endif()
        target_link_libraries(sygaldry-bindings INTERFACE sygaldry-bindings-portable)

        if (ESP_PLATFORM)
            add_library(sygaldry-bindings-esp32 INTERFACE)
                add_subdirectory(sygaldry/bindings/esp32/libmapper-arduino)
                add_subdirectory(sygaldry/bindings/esp32/spiffs)
                add_subdirectory(sygaldry/bindings/esp32/wifi)
            target_link_libraries(sygaldry-bindings INTERFACE sygaldry-bindings-esp32)
        endif()

    target_link_libraries(sygaldry-components INTERFACE sygaldry-bindings)

    add_library(sygaldry-utility_components INTERFACE)

        add_subdirectory(sygaldry/utility_components/portable/basic_logger/basic_logger)
        if (SYGALDRY_BUILD_TESTS)
            add_subdirectory(sygaldry/utility_components/portable/basic_logger/test_logger)
        endif()
        add_subdirectory(sygaldry/utility_components/portable/basic_logger/cstdio_logger)

    target_link_libraries(sygaldry-components INTERFACE sygaldry-utility_components)

target_link_libraries(sygaldry INTERFACE sygaldry-components)
