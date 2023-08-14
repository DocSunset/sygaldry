\page comp_i2c_esp32 ESP32 I2C

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

This simple wrapper component simply initializes the Arduino TwoWire API I2C
bus using the given template parameters for pin numbers and frequency.

```cpp
// @#'i2c.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "Wire.h"
#include "sygaldry-helpers-metadata.hpp"

namespace sygaldry { namespace components { namespace esp32 {

template<int sda_pin, int scl_pin, uint32_t frequency = 400000>
struct I2C
: name_<"I2C">
, description_<"the ESP32 I2C bus driver">
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
