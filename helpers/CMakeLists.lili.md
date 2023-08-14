\page helpers_cmake Helpers Library CMake

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes the CMake script for the helpers library.

Most of the functionality of the helpers library is header-only compile time
code that can't easily be physically isolated from its point of use due to the
implementation strategy. As such, the library's main function is to collect the
include directories that need to be exposed to clients of the library.

```cmake
# @#'CMakeLists.txt'
add_library(sygaldry-helpers INTERFACE)
target_link_libraries(sygaldry-helpers INTERFACE sygaldry-utilities)
target_include_directories(sygaldry-helpers INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
add_library(Sygaldry::Helpers ALIAS sygaldry-helpers)

if (SYGALDRY_BUILD_TESTS)
add_subdirectory("tests/metadata")
add_subdirectory("tests/endpoints")
add_subdirectory("tests/mimu")
endif()
# @/
```
