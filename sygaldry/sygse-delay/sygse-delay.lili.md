\page page-sygsa-delay Sygaldry Delay for Arduino

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Sygaldry portable `delay` implementation in terms of Arduino delay. See also
\ref page-sygsp-delay.

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

void delay(unsigned long ms)
{
    ::delay(ms);
}

} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygse-delay)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib} PUBLIC sygsp-delay sygse-arduino_hack)
# @/
```
