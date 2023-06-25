# Liblo OSC Binding

This document describes the implementation of the liblo OSC binding component.

```
@='license notice'
Copyright 2023 Travis J. West

This program (liblo.lili and liblo.hpp) is free software: you can redistribute
it and/or modify it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
@/
```

[TOC]

# Overview

The liblo binding has the following responsibilities:

- Create at least one `lo_address`.
- Start and manage interaction with the liblo server thread
    - including creating the thread with `lo_server_thread_new`,
    - registering an error handler callback with the same,
    - registering at least one handler with `lo_server_thread_add_method`,
    - and starting the thread with `lo_server_thread_start`.
- Route incoming messages to the appropriate endpoints, e.g. within a server
  thread method or methods
- Recognize when output signals have changed, and produce output messages
  accordingly using `lo_send` or another method

# Limitations

Liblo requires the availability of POSIX networking and pthreads APIs. This is
currently available on most personal computer operating systems, SOC embedded
processors such as Raspberry Pi and Bela, as well as ESP32, but not on most MCU
platforms. Due to discrepancies in the quality of the timebase on these
platforms, especially those that may or may not have any form of clock
synchronization available such as the ESP32, OSC functionality relying on
accurate time stamps may not behave as intended.

# Init

# Liblo OSC Binding Summary

```cpp
// @#'liblo.hpp'
/*
@{copyright statement}
*/
#pragma once
#include <lo.h>
#include <lo_serverthread.h>
#include <lo_types.h>

namespace sygaldry { namespace bindings {

template<typename Components>
struct LibloOsc
: name_<"Liblo OSC">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Travis J. West">
, license_<"SPDX-License-Identifier: LGPL-2.1-or-later"
, version_<"0.0.0">
, description_<"Open Sound Control bindings using the liblo library">
{
    void init(Components& components)
    {
        @{init}
    }

    void main(Components& components)
    {
        @{main}
    }
};

} }
// @/
```

```cpp
// @#'tests/liblo/demo.cpp'
#include "bindings/liblo.hpp"
#include "components/tests/testcomponent.hpp"

struct components_t
{
    struct api_t
    {
        TestComponent tc;
    } api;
    LibloOsc<decltype(api)> lo;
} components;

void main()
{
    init(components);

    for (;;)
    {
        activate(components);
    }
}
// @/
```

```cmake
# @#'tests/liblo/CMakeLists.txt'
# https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBLO REQUIRED liblo)
add_executable(liblo-demo demo.cpp)
target_link_libraries(liblo-demo PRIVATE ${LIBLO_LIBRARIES})
target_include_directories(liblo-demo PRIVATE ${LIBLO_INCLUDE_DIRS})
# @/
```
