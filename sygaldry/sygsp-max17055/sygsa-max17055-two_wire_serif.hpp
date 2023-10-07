/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>
#include <iostream>

namespace sygaldry { namespace sygsa {

namespace detail {
struct MAX17055TwoWireSerif
{
    static uint8_t readReg16Bit(uint8_t reg, uint8_t i2c_address);
    static void writeReg16Bit(uint8_t reg, uint16_t value, uint8_t i2c_address);
    static bool writeVerifyReg16Bit(uint8_t reg, uint16_t value , uint8_t i2c_address);
};
}

// *a very simple template*
/*! Serial interface for MAX17055 using the Arduino TwoWire API

\tparam i2c_address The I2C address of the MIMU device
*/
template<uint8_t i2c_address>
struct MAX17055TwoWireSerif
{
    /// Read 16 bit register
    static uint8_t readReg16Bit(uint8_t reg, uint8_t i2c_address)
    {
        return detail::MAX17055TwoWireSerif::readReg16Bit(uint8_t reg, uint8_t i2c_address);
    }

    /// Write to 16 bit register
    static void writeReg16Bit(uint8_t reg, uint16_t value, uint8_t i2c_address)
    {
        detail::MAX17055TwoWireSerif::writeReg16Bit(uint8_t reg, uint16_t value, uint8_t i2c_address);
    }

    /// Write and verify to 16 bit register
    static bool writeVerifyReg16Bit(uint8_t reg, uint16_t value , uint8_t i2c_address)
    {
        detail::MAX17055TwoWireSerif::writeVerifyReg16Bit(uint8_t reg, uint16_t value, uint8_t i2c_address);
    }
};

// *explicit external instantiations*
extern template struct MAX17055TwoWireSerif<0x36>;

} }
