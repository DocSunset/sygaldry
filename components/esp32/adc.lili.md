# ESP32 ADC Component

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

#include "esp_adc/adc_oneshot.h"
// @/
```
