\page page-docs-build_system The Build System

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes the Sygaldry build system.

The build environment setup is automated using the Nix package manager. See
\subpage page-docs-nix_shell for a detailed description.

# Sygaldry root

We set a variable allowing components to reach the root of the repository. This
is mainly done so that components can reach the `dependencies` directory
unambiguously as `${SYGALDRY_ROOT}/dependencies`, rather than using a relative
path that might break if the component is physically moved.

```cmake
# @='set SYGALDRY_ROOT'
set(SYGALDRY_ROOT ${CMAKE_CURRENT_LIST_DIR})
# @/
```

## Language Standard

We require C++20 without extensions.

```cmake
# @='set language standard'
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
# @/
```

## Testing Framework

Catch2 is used for unit testing on platforms where it can run. Other platforms
may use a different testing methodology.

```cmake
# @='include FetchContent'
Include(FetchContent)
# @/

# @='Fetch Catch2'
find_package(Catch2 3 REQUIRED)
# @/

# @='Include automatic test registration'
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
# @/
```

Catch2 is not a good testing framework for ESP-IDF and Pico SDK, so the code to
enable it is disabled for those platforms.

```cmake
# @='prepare for tests'
if (NOT ESP_PLATFORM AND NOT PICO_SDK)
    @{Fetch Catch2}
    @{Include automatic test registration}
    set(SYGALDRY_BUILD_TESTS 1)
endif()
# @/
```

See [the Catch2 CMake integration documentation](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#top) for more detail.

## Avendish

Avendish is used to build Pd externals, and eventually perhaps for other
bindings. It is currently incompatible with ESP-IDF and Pico SDK.

```cmake
# @='fetch Avendish'
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
# @/
```

## CMake Enabled Libraries

Boost PFR and Boost MP11 are required by the concepts library, and consequently
by any bindings or components that make use of it. Several other components
make use of specific libraries, such as liblo and `Trill-Arduino`; some of
these are also included as submodules, and some may be required to build the
default test suite. Since many of these dependencies do not provide cmake
support, they are handled on a case by case basis in the `CMakeLists.txt` files
of the components that use them. Those that can be included via CMake are made
available here, when appropriate depending on the platform.

```cmake
# @='include cmake libraries'
add_subdirectory(dependencies/pfr)
add_subdirectory(dependencies/mp11)
add_library (eigen INTERFACE)
add_library (Eigen3::Eigen ALIAS eigen)
target_include_directories (eigen INTERFACE dependencies/eigen)
# @/
```

## Software Components

The project is physically structured as self-contained software components, in
the sense described by John Lakos \cite lakos2019large-scale-cpp. Each
component lives in its own directory, with its own CMakeLists.txt file. The
main CMakeLists.txt file's principal job is to include these components as
subdirectories.

In an earlier version of the project, there was a directory structure
resembling the following, where each leaf node was a directory containing a
component as just described:

```
sygaldry
*-- bindings
|   *-- esp32
|   |   *-- libmapper-arduino
|   |   *-- spiffs
|   |   *-- etc.
|   *-- portable
|       *-- cli
|       *-- output_logger
|       *-- etc.
*-- concepts
|   *-- components
|   *-- endpoints
|   *-- etc.
*-- helpers
|   *-- endpoints
|   *-- metadata
|   *-- etc.
*-- sensors
    *-- arduino
    |   *-- icm20948
    |   *-- trill_craft
    |   *-- etc.
    *-- esp32
    |   *-- adc
    |   *-- arduino-hack
    |   *-- etc.
    *-- portable
        *-- etc.
```

Components were correspondingly named e.g. `sygaldry-sensors-esp32-adc`.

As well as being troublingly verbose when editing, this proved problematic when
building on Windows, which at the time in the year 2023 enforced a limit of 260
characters on the length of a path when making a directory. Some instrument
firmwares being built at the time would include the main repository root as a
library, resulting in very long build artefact paths such as
`C:\Users\user\Github\sygaldry\sygaldry\instruments\instrument_name\instrument_name_platform\instrument\build\main\sygbuild\C_Users\user\Github\sygaldry\sensors\arduino\trill_craft\sygaldry-sensors-arduino-trill_craft.obj.d`
that would eventually trip the path length limit. Although it was possible to
modify registry keys and otherwise work around the issue, given the
inconvenience entailed in editing and maintaining the long component names, it
was decided to adopt an organizational scheme with better characteristics in
terms of brevity.

The repository now contains two main content directories: `sygaldry` and
`sygaldry-instruments`. The `sygaldry` directory contains all of the software
components in the library, except for the instruments, which are unsurprisingly
found in the other directory. Rather than being named verbosely, in keeping
with \cite lakos2019large-scale-cpp, packages and their components are given
identifiers of the form `sygXY` where `XY` is the package identifier, with `X`
typically (currently always) referring to the broad type of components and `Y`
to the platform.

# Authoritative Component List

The authoritative list of components is automatically generated by
\ref page-sh-generate_components_cmake and placed at `sygaldry/CMakeLists.txt`.
We add that subdirectory here, as well as instruments that should be built with
the test suite.

```cmake
# @='add subdirectories'
add_subdirectory(sygaldry)
add_subdirectory(sygaldry-instruments/test)
# @/
```

# Summary

```cmake
# @#'CMakeLists.txt'
# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.24)
project(Sygaldry)

@{set SYGALDRY_ROOT}
@{set language standard}
@{include FetchContent}

@{prepare for tests}

@{fetch Avendish}

@{include cmake libraries}

@{add subdirectories}
# @/
```
