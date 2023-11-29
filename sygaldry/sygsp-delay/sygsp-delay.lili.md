\page page-sygsp-delay sygsp-delay: Delay

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Arduino-style `delay` with more encapsulation and fewer unwanted dependencies.

This component exists so that, rather than rely directly on the Arduino API, a
component can access this functionality through an interface that we can
directly control. This should make it easier to port such a component to a
platform for which Arduino support is not available. Unless a dependency on
another API is already required for some reason, this API should be preferred
over the equivalent Arduino API or an API provided by a platform-specific
library.

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

/*! \addtogroup sygsp
*/
/// \{

/*! \defgroup sygsp-delay sygsp-delay: Delay
This software component provides a portable API for a simple delay function,
equivalent to the Arduino `delay` API, but with better encapsulation.
*/
/// \{

/*! Delay execution for a certain amount of time in milliseconds.

This function should be defined by each platform. Clients of the API will
need to link the platform-specific library as well as including the header
in which this function is declared.

This API is currently implemented for ESP-IDF and Raspbery Pi Pico SDK via
the [Arduino hack subsystem](\ref page-sygsa-delay).
*/
void delay(unsigned long ms);

/// \}
/// \}

} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsp-delay)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
# @/
```

