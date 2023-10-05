\page page-sygsa-two_wire Arduino TwoWire Component

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

This simple wrapper component simply initializes the Arduino TwoWire API I2C
bus using the given template parameters for pin numbers and frequency.

```cpp
// @#'sygsa-two_wire.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "Wire.h"
#include "sygah-metadata.hpp"

namespace sygaldry { namespace components { namespace arduino {

template<int sda_pin, int scl_pin, uint32_t frequency = 100000>
struct TwoWire
: name_<"Two Wire Bus">
, description_<"the Arduino I2C bus driver">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Travis J. West">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    void init()
    {
        Wire.begin(sda_pin, scl_pin, frequency);
    }
};

} } }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsa-two_wire)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygah-metadata)

target_link_libraries(sygsa INTERFACE ${lib})
# @/
```
