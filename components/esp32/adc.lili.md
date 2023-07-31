\page comp_adc_esp32 ESP32 ADC Component

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

The ADC component provides a very basic wrapper around the one-shot ADC read
API provided by the ESP-IDF; it is essentially a wrapper around the
[`oneshot_read_main.c` example](https://github.com/espressif/esp-idf/blob/v5.1-rc1/examples/peripherals/adc/oneshot_read/main/oneshot_read_main.c)
provided with the IDF documentation.

[TOC]

# Implementation

## Init

## Main

# Summary

```cpp
// @#'adc.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "esp_adc/adc_oneshot.h"
// @/
```
