\page page-sygse-micros Portable Timestamp API

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Sygaldry portable `micros` implementation for ESP-IDF via the Arduino Hack
subsystem.

```cpp
// @#'sygse-micros.cpp'
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "Arduino.h"

namespace sygaldry { namespace sygsp {

unsigned long micros()
{
    return ::micros();
}

} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygse-micros)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib} PUBLIC sygsp-micros sygse-arduino_hack)
# @/
```

