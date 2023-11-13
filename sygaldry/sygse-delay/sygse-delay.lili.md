\page page-sygse-delay sygse-delay: Sygaldry Delay for ESP32

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Sygaldry portable `delay` implementation in terms of Arduino delay on ESP32.
See also \ref page-sygsp-delay.

```cpp
// @#'sygse-delay.cpp'
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "Arduino.h"

namespace sygaldry { namespace sygsp {
///\addtogroup sygse
///\{
///\defgroup sygse-delay sygse-delay: Sygaldry Delay for ESP32
///\{

void delay(unsigned long ms)
{
    ::delay(ms);
}

///\}
///\}
} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygse-delay)
add_library(${lib} INTERFACE)
target_sources(${lib} INTERFACE ${lib}.cpp)
target_link_libraries(${lib} INTERFACE sygsp-delay sygse-arduino_hack)
# @/
```
