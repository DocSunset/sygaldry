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
target_include_directories(sygaldry-utilities INTERFACE)
add_subdirectory(utilities/consteval)
target_link_libraries(sygaldry INTERFACE sygaldry-utilities)

add_library(sygaldry-concepts INTERFACE)
add_subdirectory(concepts/components)
add_subdirectory(concepts/endpoints)
add_subdirectory(concepts/functions)
add_subdirectory(concepts/metadata)
add_subdirectory(concepts/mimu)
add_subdirectory(concepts/runtime)
target_link_libraries(sygaldry INTERFACE sygaldry-concepts)

#add_library(sygaldry-helpers INTERFACE)
add_subdirectory(helpers)
target_link_libraries(sygaldry INTERFACE sygaldry-helpers)

add_library(sygaldry-bindings INTERFACE)
add_subdirectory(bindings)
target_link_libraries(sygaldry INTERFACE sygaldry-bindings)

add_library(sygaldry-components INTERFACE)
add_subdirectory(components)
target_link_libraries(sygaldry INTERFACE sygaldry-components)
