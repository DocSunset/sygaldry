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
    find_package(Catch2 3 REQUIRED)
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
add_library (eigen INTERFACE)
add_library (Eigen3::Eigen ALIAS eigen)
target_include_directories (eigen INTERFACE dependencies/eigen)

add_subdirectory(sygaldry)
add_subdirectory(sygaldry-instruments/test)
