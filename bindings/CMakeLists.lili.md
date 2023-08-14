\page bind_cmake Bindings Library CMake

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes the CMake script for the bindings library.

```cmake
# @#'CMakeLists.txt'
add_library(sygaldry_bindings STATIC src/osc_match_pattern.cpp)
target_link_libraries(sygaldry_bindings PUBLIC sygaldry-concepts)
target_link_libraries(sygaldry_bindings PUBLIC Sygaldry::Helpers)
target_include_directories(sygaldry_bindings PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
add_library(Sygaldry::Bindings ALIAS sygaldry_bindings)

if (SYGALDRY_BUILD_TESTS)
add_subdirectory(tests/spelling)
add_subdirectory(tests/osc_string_constants)
add_subdirectory(tests/output_logger)
add_subdirectory(tests/osc_match_pattern)
add_subdirectory(tests/liblo)
add_subdirectory(tests/rapidjson)
add_subdirectory(tests/basic_logger)
add_subdirectory(tests/basic_reader)
add_subdirectory(tests/cli)
add_subdirectory(tests/demo)
endif()
# @/
```
