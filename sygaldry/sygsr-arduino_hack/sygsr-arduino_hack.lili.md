\page page-sygsr-arduino_hack sygsr-arduino_hack: RP2040 Arduino Hack

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later

This document describes the RP2040 implementation of the Arduino APIs supported
by Sygaldry's Arduino hack subsystem. Although an Arduino library implementation
exists for ESP32, it tends to lag behind the main ESP-IDF version. At the time
of writing, the ESP32 Arduino library is not available with a compiler that is
compatible with Sygaldry, so Arduino hack is necessary.

An IDF component `CMakeLists.txt` registers the implementation with the
framework, and an implementation of the `TwoWire` API is provided.

# ESP Component CMakeLists.txt

```cmake
# @#'CMakeLists.txt'
set(lib sygsr-arduino_hack)
add_library(${lib} INTERFACE)
target_link_libraries(${lib} INTERFACE sygsp-arduino_hack)
target_link_libraries(${lib} INTERFACE hardware_timer hardware_i2c)
target_sources(${lib} INTERFACE Arduino.cpp Wire.cpp)
# @/
```

# ESP Arduino.h

```cpp
// @#'Arduino.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "Arduino.h"
#include "hardware/timer.h"

void pinMode(uint8_t pin, uint8_t mode)
{
}

void digitalWrite(uint8_t pin, uint8_t val)
{
}

unsigned long micros()
{
    return time_us_64();
}

void delay(unsigned long ms)
{
    busy_wait_ms(ms);
}
// @/
```

# RP2040 TwoWire API

We define a very simple implementation of this API in terms of the the Pico
SDK. It remains as future work to properly document this implementation, but it
is hoped that the reader will find it reasonably straightforward.

```cpp
// @#'Wire.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "Wire.h"
#include "hardware/i2c.h"

namespace
{
    static uint8_t _tx_address = 0;
    static uint8_t _tx_idx = 0;
    static uint8_t _tx_buffer[16] = {0};
    static uint8_t _rx_idx = 0;
    static uint8_t _rx_length = 0;
    static uint8_t _rx_buffer[BUFFER_LENGTH] = {0};
    static bool _repeated_start = false;
    static constexpr unsigned int _timeout = 20*1000;
}

TwoWire::TwoWire()
{
};

void TwoWire::begin()
{
    // there's no way without arduino to know what pins to use...
    // so the user should really call begin() manually before calling
    // Trill::begin() or anything else that calls Wire::begin()...
}

void TwoWire::begin(int sda_pin, int scl_pin, uint32_t frequency)
{
    printf("Wire: begin(%d, %d, %ld)\n", sda_pin, scl_pin, frequency);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    i2c_init(&i2c0_inst, frequency);
}

void TwoWire::beginTransmission(uint8_t address)
{
    //printf("beginTransmission %x\n", address);
    _tx_address = address;
}

void TwoWire::write(uint8_t b)
{
    //printf("write %x\n", b);
    _tx_buffer[_tx_idx++] = b;
}

void TwoWire::write(uint8_t * buffer, uint8_t length)
{
    //printf("write "); for (int i = 0; i < length; ++i) printf("%x ", buffer[i]); printf("\n");
    for (std::size_t i = 0; i < length; ++i) write(buffer[i]);
}

void TwoWire::endTransmission(bool sendStop)
{
    //printf("endTransmission %d\n", sendStop);
    int bytes_written = i2C_write_timeout_us(i2c0_inst, _tx_address, _tx_buffer, _tx_idx, sendStop, _timeout);
    //printf("justwrite: tx - ");
    //for (int i = 0; i < _tx_idx; ++i)
    //    printf("%d ", _tx_buffer[i]);
    //printf("\n");
    switch (bytes_written)
    {
    case PICO_ERROR_GENERIC:
        printf("TwoWire::endTransmission: failure; no subnode ACK\n");
        break;
    case PICO_ERROR_TIMEOUT:
        printf("TwoWire::endTransmission: failure; timeout\n");
        break;
    }
    _tx_idx = 0;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t length)
{
    //printf("requestFrom %x %d %d\n", address, length, _repeated_start);
    int bytes_read = i2c_read_timeout_us(_port, address, _rx_buffer, length, true,_timeout);
    switch(bytes_read)
    {
    case PICO_ERROR_GENERIC:
        printf("TwoWire::requestFrom: failure; no subnode ACK\n");
        return 0;
    case PICO_ERROR_TIMEOUT:
        printf("TwoWire::requestFrom: i2c bus timeout\n");
        return 0;
    default:
        _rx_idx = 0;
        _rx_length = bytes_read;
        return bytes_read;
    }
}

uint8_t TwoWire::available()
{
    return _rx_length - _rx_idx;
}

uint8_t TwoWire::read()
{
    if (_rx_idx < _rx_length)
        return _rx_buffer[_rx_idx++];
    else return 0;
}

TwoWire Wire{};
// @/
```
