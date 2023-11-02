\page page-sygsa-two_wire_serif sygsa-two_wire_serif: TwoWire Byte Serif

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes the implementation of [the byte-wise serial
interface](\ref page-sygsp-byte_serif) using the Arduino `TwoWire` API.

As well as a straightforward `struct` that requires the I2C address to be
passed, we define *a very simple template* that passes this information as a
compile-time non-type template parameter. We declare *explicit external
instantiations* of this template for the two default I2C addresses and compile
them in the implementation file for the `Serif`.

```cpp
// @#'sygsa-two_wire_serif.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>

namespace sygaldry { namespace sygsa {

/// \addtogroup sygsp-icm20948
/// \{
/// \defgroup sygsa-two_wire_serif sygsa-two_wire_serif: Arduino Serial Interface for ICM20948 MIMU
/// \{

namespace detail {
struct TwoWireByteSerif
{
    [[nodiscard]] static uint8_t read(uint8_t i2c_address, uint8_t register_address);
    static uint8_t read(uint8_t i2c_address, uint8_t register_address, uint8_t * buffer, uint8_t bytes);
    static void write(uint8_t i2c_address, uint8_t register_address, uint8_t value);
};
}

// *a very simple template*
/*! Serial interface for ICM20948 using the Arduino TwoWire API

\tparam i2c_address The I2C address of the MIMU device
*/
template<uint8_t i2c_address>
struct TwoWireByteSerif
{
    /// Read one byte and return it
    [[nodiscard]] static uint8_t read(uint8_t register_address)
    {
        return detail::TwoWireByteSerif::read(i2c_address, register_address);
    }

    /// Read many bytes; returns the number of bytes read
    static uint8_t read(uint8_t register_address, uint8_t * buffer, uint8_t bytes)
    {
        return detail::TwoWireByteSerif::read(i2c_address, register_address, buffer, bytes);
    }

    /// Write one byte
    static void write(uint8_t register_address, uint8_t value)
    {
        detail::TwoWireByteSerif::write(i2c_address, register_address, value);
    }
};

/// \}
/// \}

} }
// @/
```

The definition of these functions involves very typical use of the `TwoWire` API.
We define the single byte read in terms of the multi byte read.

```cpp
// @#'sygsa-two_wire_serif.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsa-two_wire_serif.hpp"
#include <Wire.h>

namespace sygaldry { namespace sygsa { namespace detail {

uint8_t TwoWireByteSerif::read(uint8_t i2c_address, uint8_t register_address)
{
    uint8_t out = 0;
    read(i2c_address, register_address, &out, 1);
    return out;
}

uint8_t TwoWireByteSerif::read(uint8_t i2c_address, uint8_t register_address, uint8_t * buffer, uint8_t bytes)
{
    Wire.beginTransmission(i2c_address);
    Wire.write(register_address);
    Wire.endTransmission(false); // repeated start
    Wire.requestFrom(i2c_address, bytes);
    for (uint8_t i = 0; i < bytes; ++i)
    {
        if (Wire.available()) buffer[i] = Wire.read();
        else return i;
    }
    return bytes;
}

void TwoWireByteSerif::write(uint8_t i2c_address, uint8_t register_address, uint8_t value)
{
    Wire.beginTransmission(i2c_address);
    Wire.write(register_address);
    Wire.write(value);
    Wire.endTransmission();
}

} } }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsa-two_wire_serif)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_include_directories(${lib} PUBLIC .)
target_link_libraries(${lib} PUBLIC sygsp-arduino_hack)
# @/
```

