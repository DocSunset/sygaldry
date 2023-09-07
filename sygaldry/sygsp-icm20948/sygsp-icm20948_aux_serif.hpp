/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>
#include "sygsp-delay.hpp"
#include "sygsp-icm20948_registers.hpp"

namespace sygaldry { namespace sygsp {

/*! Limited I2C serial interface using the ICM20948 aux bus

\pre

This API requires that the ICM20948 accessible through the given serial
interface should be connecting, accessible, not sleeping, and not in low power
mode mode. The user is requird to check or enforce this precondition, since
this API does not.

\tparam Serif the serial interface over which to access the ICM20948
*/
template<typename Serif, uint8_t i2c_address>
struct ICM20948AuxSerif
{
    using Registers = ICM20948Registers<Serif>;

    /// Read one byte and return it
    [[nodiscard]] static uint8_t read(uint8_t register_address)
    {
        Registers::I2C_SLV4_ADDR::write(1<<8 | i2c_address);
        Registers::I2C_SLV4_REG::write(register_address);
        Registers::I2C_SLV4_CTRL::I2C_SLV4_EN::trigger();
        delay(1);
        return Registers::I2C_SLV4_DI::read();
    }

    /// Write one byte
    static void write(uint8_t register_address, uint8_t value)
    {
        Registers::I2C_SLV4_ADDR::write(1<<8 | i2c_address);
        Registers::I2C_SLV4_REG::write(register_address);
        Registers::I2C_SLV4_DO::write(value);
        Registers::I2C_SLV4_CTRL::I2C_SLV4_EN::trigger();
    }
};
} }
