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

namespace detail {
struct MAX17055TwoWireSerif
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
struct MAX17055TwoWireSerif
{
    /// Read one byte and return it
    [[nodiscard]] static uint8_t read(uint8_t register_address)
    {
        return detail::MAX170555TwoWireSerif::read(i2c_address, register_address);
    }

    /// Read many bytes; returns the number of bytes read
    static uint8_t read(uint8_t register_address, uint8_t * buffer, uint8_t bytes)
    {
        return detail::MAX17055TwoWireSerif::read(i2c_address, register_address, buffer, bytes);
    }

    /// Write one byte
    static void write(uint8_t register_address, uint8_t value)
    {
        detail::MAX17055TwoWireSerif::write(i2c_address, register_address, value);
    }
};

// *explicit external instantiations*
extern template struct MAX17055TwoWireSerif<0x36>;

} }
