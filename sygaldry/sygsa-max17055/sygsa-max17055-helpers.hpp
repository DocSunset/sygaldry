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
/// Read 16 bit register
static uint8_t readReg16Bit(uint8_t i2c_address, uint8_t reg);
/// Write to 16 bit register
static void writeReg16Bit(uint8_t i2c_address, uint8_t reg, uint16_t valu);
/// Write and verify to 16 bit register
static bool writeVerifyReg16Bit(uint8_t i2c_address, uint8_t reg, uint16_t value);
}
}