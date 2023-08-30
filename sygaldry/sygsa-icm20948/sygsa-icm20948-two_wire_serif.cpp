/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsa-icm20948-two_wire_serif.hpp"
#include <Wire.h>

namespace sygaldry { namespace sygsa {

namespace detail {
uint8_t ICM20948TwoWireSerif::read(uint8_t i2c_address, uint8_t register_address)
{
    uint8_t out = 0;
    read(i2c_address, register_address, &out, 1);
    return out;
}

uint8_t ICM20948TwoWireSerif::read(uint8_t i2c_address, uint8_t register_address, uint8_t * buffer, uint8_t bytes)
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

void ICM20948TwoWireSerif::write(uint8_t i2c_address, uint8_t register_address, uint8_t value)
{
    Wire.beginTransmission(i2c_address);
    Wire.write(register_address);
    Wire.write(value);
    Wire.endTransmission();
}
}

extern template struct ICM20948TwoWireSerif<0b1101000>;
extern template struct ICM20948TwoWireSerif<0b1101001>;

} }
