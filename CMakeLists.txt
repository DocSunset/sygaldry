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
Include(FetchContent)

if (NOT ESP_PLATFORM AND NOT PICO_SDK)
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

if (NOT ESP_PLATFORM AND NOT PICO_SDK)
FetchContent_Declare(
  avendish
  GIT_REPOSITORY "https://github.com/celtera/avendish"
  GIT_TAG  3b3bd7b2ecf2061900726100e664b69c51b8e402
  GIT_PROGRESS true
)
FetchContent_Populate(avendish)

set(CMAKE_PREFIX_PATH "${avendish_SOURCE_DIR};${CMAKE_PREFIX_PATH}")
find_package(Avendish REQUIRED)
set(SYGALDRY_BUILD_AVENDISH 1)
endif()

add_subdirectory(dependencies/pfr)
add_subdirectory(dependencies/mp11)
if(PICO_SDK)
    # on Raspberry Pi Pico SDK, the upsteam Eigen3 cmake trips an error,
    # see https://gitlab.com/libeigen/eigen/-/issues/2740
    # for now, we work around this by manually defining the Eigen library with
    # a compatible name, alias, etc.
    set ( EIGEN_DEFINITIONS "")
    add_library (eigen INTERFACE)
    add_library (Eigen3::Eigen ALIAS eigen)
    target_compile_definitions (eigen INTERFACE ${EIGEN_DEFINITIONS})
    target_include_directories (eigen INTERFACE $ENV{SYGALDRY_ROOT}/dependencies/eigen)
else()
    set(EIGEN_BUILD_TESTING FALSE)
    add_subdirectory(dependencies/eigen)
endif()

add_library(sygaldry INTERFACE)

add_library(sygac INTERFACE)
                add_subdirectory(sygaldry/sygac-components)
    target_link_libraries(sygac INTERFACE sygac-components)
                add_subdirectory(sygaldry/sygac-endpoints)
    target_link_libraries(sygac INTERFACE sygac-endpoints)
                add_subdirectory(sygaldry/sygac-functions)
    target_link_libraries(sygac INTERFACE sygac-functions)
                add_subdirectory(sygaldry/sygac-metadata)
    target_link_libraries(sygac INTERFACE sygac-metadata)
                add_subdirectory(sygaldry/sygac-mimu)
    target_link_libraries(sygac INTERFACE sygac-mimu)
                add_subdirectory(sygaldry/sygac-runtime)
    target_link_libraries(sygac INTERFACE sygac-runtime)
target_link_libraries(sygaldry INTERFACE sygac)

add_library(sygah INTERFACE)
                add_subdirectory(sygaldry/sygah-consteval)
    target_link_libraries(sygah INTERFACE sygah-consteval)
                add_subdirectory(sygaldry/sygah-endpoints)
    target_link_libraries(sygah INTERFACE sygah-endpoints)
                add_subdirectory(sygaldry/sygah-string_literal)
    target_link_libraries(sygah INTERFACE sygah-string_literal)
                add_subdirectory(sygaldry/sygah-metadata)
    target_link_libraries(sygah INTERFACE sygah-metadata)
                add_subdirectory(sygaldry/sygah-mimu)
    target_link_libraries(sygah INTERFACE sygah-mimu)
target_link_libraries(sygaldry INTERFACE sygah)

add_library(sygsp INTERFACE)
    # arduino-hack, alphabetically missing here, is added only where required by a platform
                add_subdirectory(sygaldry/sygsp-button)
    target_link_libraries(sygsp INTERFACE sygsp-button)
                add_subdirectory(sygaldry/sygsp-delay)
    target_link_libraries(sygsp INTERFACE sygsp-delay)
                add_subdirectory(sygaldry/sygsp-micros)
    target_link_libraries(sygsp INTERFACE sygsp-micros)
                add_subdirectory(sygaldry/sygsp-mimu_units)
    target_link_libraries(sygsp INTERFACE sygsp-mimu_units)
                add_subdirectory(sygaldry/sygsp-complementary_mimu_fusion)
    target_link_libraries(sygsp INTERFACE sygsp-complementary_mimu_fusion)
target_link_libraries(sygaldry INTERFACE sygsp)

if (ESP_PLATFORM)
    add_library(sygse INTERFACE)
    add_library(sygsa INTERFACE)
                    add_subdirectory(sygaldry/sygsp-arduino_hack)
        target_link_libraries(sygsp INTERFACE sygsp-arduino_hack)
                    add_subdirectory(sygaldry/sygse-arduino_hack)
        target_link_libraries(sygse INTERFACE sygse-arduino_hack)
                    add_subdirectory(sygaldry/sygsa-two_wire)
        target_link_libraries(sygsa INTERFACE sygsa-two_wire)
                    add_subdirectory(sygaldry/sygsa-trill_craft)
        target_link_libraries(sygsa INTERFACE sygsa-trill_craft)
                    add_subdirectory(sygaldry/sygse-delay)
        target_link_libraries(sygsa INTERFACE sygse-delay)
                    add_subdirectory(sygaldry/sygse-micros)
        target_link_libraries(sygsa INTERFACE sygse-micros)
                    add_subdirectory(sygaldry/sygse-adc)
        target_link_libraries(sygse INTERFACE sygse-adc)
                    add_subdirectory(sygaldry/sygse-button)
        target_link_libraries(sygse INTERFACE sygse-button)
                    add_subdirectory(sygaldry/sygse-gpio)
        target_link_libraries(sygse INTERFACE sygse-gpio)
                    add_subdirectory(sygaldry/sygsp-icm20948)
        target_link_libraries(sygse INTERFACE sygsp-icm20948)
                    add_subdirectory(sygaldry/sygse-trill)
        target_link_libraries(sygse INTERFACE sygse-trill)
    target_link_libraries(sygaldry INTERFACE sygse)
    target_link_libraries(sygaldry INTERFACE sygsa)
endif()

if (PICO_SDK)
    add_library(sygsr INTERFACE)
                    add_subdirectory(sygaldry/sygsr-button)
        target_link_libraries(sygsr INTERFACE sygsr-button)
    target_link_libraries(sygaldry INTERFACE sygsr)
endif()

add_library(sygbp INTERFACE)
                add_subdirectory(sygaldry/sygbp-cstdio_reader)
    target_link_libraries(sygbp INTERFACE sygbp-cstdio_reader)
                add_subdirectory(sygaldry/sygbp-cli)
    target_link_libraries(sygbp INTERFACE sygbp-cli)
                add_subdirectory(sygaldry/sygbp-liblo)
    target_link_libraries(sygbp INTERFACE sygbp-liblo)
                add_subdirectory(sygaldry/sygbp-osc_match_pattern)
    target_link_libraries(sygbp INTERFACE sygbp-osc_match_pattern)
                add_subdirectory(sygaldry/sygbp-osc_string_constants)
    target_link_libraries(sygbp INTERFACE sygbp-osc_string_constants)
                add_subdirectory(sygaldry/sygbp-output_logger)
    target_link_libraries(sygbp INTERFACE sygbp-output_logger)
                add_subdirectory(sygaldry/sygbp-rapid_json)
    target_link_libraries(sygbp INTERFACE sygbp-rapid_json)
                add_subdirectory(sygaldry/sygbp-session_data)
    target_link_libraries(sygbp INTERFACE sygbp-session_data)
                add_subdirectory(sygaldry/sygbp-spelling)
    target_link_libraries(sygbp INTERFACE sygbp-spelling)
                add_subdirectory(sygaldry/sygbp-test_component)
    target_link_libraries(sygbp INTERFACE sygbp-test_component)
    if (SYGALDRY_BUILD_TESTS)
                    add_subdirectory(sygaldry/sygbp-test_reader)
        target_link_libraries(sygbp INTERFACE sygbp-test_reader)
    endif()
target_link_libraries(sygaldry INTERFACE sygbp)

if (ESP_PLATFORM)
add_library(sygbe INTERFACE)

                add_subdirectory(sygaldry/sygbe-runtime)
    target_link_libraries(sygbe INTERFACE sygbe-runtime)
                add_subdirectory(sygaldry/sygbe-libmapper_arduino)
    target_link_libraries(sygbe INTERFACE sygbe-libmapper_arduino)
                add_subdirectory(sygaldry/sygbe-spiffs)
    target_link_libraries(sygbe INTERFACE sygbe-spiffs)
                add_subdirectory(sygaldry/sygbe-wifi)
    target_link_libraries(sygbe INTERFACE sygbe-wifi)

target_link_libraries(sygaldry INTERFACE sygbe)
endif()

if (PICO_SDK)
add_library(sygbr INTERFACE)
                add_subdirectory(sygaldry/sygbr-runtime)
    target_link_libraries(sygbr INTERFACE sygbr-runtime)
                add_subdirectory(sygaldry/sygbr-cli)
    target_link_libraries(sygbr INTERFACE sygbr-cli)
target_link_libraries(sygaldry INTERFACE sygbr)
endif()

add_library(sygup INTERFACE)

                add_subdirectory(sygaldry/sygup-basic_logger)
    target_link_libraries(sygup INTERFACE sygup-basic_logger)
                add_subdirectory(sygaldry/sygup-cstdio_logger)
    target_link_libraries(sygup INTERFACE sygup-cstdio_logger)
    if (SYGALDRY_BUILD_TESTS)
                    add_subdirectory(sygaldry/sygup-test_logger)
        target_link_libraries(sygup INTERFACE sygup-test_logger)
    endif()

target_link_libraries(sygaldry INTERFACE sygup)

add_subdirectory(sygaldry-instruments/test)
