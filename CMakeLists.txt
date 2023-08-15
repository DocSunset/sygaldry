# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.26)
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

add_library(sygaldry-sensors INTERFACE)
    add_library(sygaldry-sensors-portable INTERFACE)
        # arduino-hack, alphabetically missing here, is added only where required by a platform
        add_subdirectory(sygaldry/sensors/portable/button)
    target_link_libraries(sygaldry-sensors INTERFACE sygaldry-sensors-portable)
    if (ESP_PLATFORM)
    add_library(sygaldry-sensors-esp32 INTERFACE)
    add_library(sygaldry-sensors-arduino INTERFACE)
        add_subdirectory(sygaldry/sensors/portable/arduino-hack)
        add_subdirectory(sygaldry/sensors/esp32/arduino-hack)
        add_subdirectory(sygaldry/sensors/arduino/two_wire)
        add_subdirectory(sygaldry/sensors/arduino/trill_craft)
        add_subdirectory(sygaldry/sensors/esp32/button)
        add_subdirectory(sygaldry/sensors/esp32/gpio)
        #add_subdirectory(sygaldry/sensors/esp32/icm20948)
        add_subdirectory(sygaldry/sensors/esp32/trill)
    target_link_libraries(sygaldry-sensors INTERFACE sygaldry-sensors-esp32)
    target_link_libraries(sygaldry-sensors INTERFACE sygaldry-sensors-arduino)
    endif()
target_link_libraries(sygaldry INTERFACE sygaldry-sensors)

add_library(sygaldry-bindings INTERFACE)
    add_subdirectory(sygaldry/bindings)
target_link_libraries(sygaldry INTERFACE sygaldry-bindings)
