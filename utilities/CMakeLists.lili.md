\page util_cmake Utilities CMake

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes the CMake script for the concepts library.

This is pretty trivial for now.

```cmake
# @#'CMakeLists.txt'
add_library(sygaldry_utilities INTERFACE)
target_include_directories(sygaldry_utilities INTERFACE "${CMAKE_CURRENT_LIST_DIR}")
add_library(Sygaldry::Utilities ALIAS sygaldry_utilities)
# @/
```
