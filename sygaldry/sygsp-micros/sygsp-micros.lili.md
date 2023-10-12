\page page-sygsp-micros sygsp-micros: Portable Timestamp API

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Arduino-style `micros` with more encapsulation and fewer unwanted dependencies.

```cpp
// @#'sygsp-delay.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

namespace sygaldry { namespace sygsp {

/// \addtogroup sygsp
/// \{

/*! \defgroup sygsp-micros sygsp-micros: Portable Timestamp API

This software component provides a portable API for a simple timestamp function,
equivalent to the Arduino `micros` API, but with better encapsulation.
*/
/// \{

/*! Get a timestamp in microseconds

This function should return a generally increasing timestamp in microseconds,
such as a count of microseconds since platform boot. The timestamp is not required
to refer to any particular epoch. The timestamp is expected to overflow periodically.

The only safe operation on the timestamps returned by this function is to
subtract an older timestamp from a newer one to get the time elapsed. This is
only safe if the time elapsed is known to be less than the platform specific
overflow period.

This function should be defined by each platform. Clients of the API will
need to link the platform-specific library as well as including the header
in which this function is declared.

This API is currently implemented for [ESP-IDF](\ref page-sygse-delay) via
the [Arduino hack subsystem](\ref page-sygse-arduino_hack).
*/
unsigned long micros();

/// \}
/// \}

} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsp-micros)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
# @/
```

