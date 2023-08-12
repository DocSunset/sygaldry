\page comp_cmake Components Library CMake

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes the CMake script for the components library.

```cmake
# @#'CMakeLists.txt'
add_library(sygaldry_components STATIC src/button.cpp)
if (ESP_PLATFORM)
target_link_libraries(sygaldry_components PUBLIC idf::trill)
target_sources(sygaldry_components PRIVATE src/trill_craft.cpp)
endif()
target_link_libraries(sygaldry_components PUBLIC Sygaldry::Helpers)
target_include_directories(sygaldry_components PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
add_library(Sygaldry::Components ALIAS sygaldry_components)
# @/
```
