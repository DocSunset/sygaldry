/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsa-max17055-two_wire_serif.hpp"
#include "sygsp-delay.hpp"
#include <Wire.h>

namespace sygaldry { namespace sygsa {

namespace detail {
uint8_t MAX17055TwoWireSerif::readReg16Bit(uint8_t reg, uint8_t i2c_address)
{
  uint16_t value = 0;  
  Wire.beginTransmission(i2c_address); 
  Wire.write(reg);
  Wire.endTransmission(false);
  
  Wire.requestFrom(i2c_address, (uint8_t) 2); 
  value  = Wire.read();
  value |= (uint16_t)Wire.read() << 8;      // value low byte
  return value;
}

void MAX17055TwoWireSerif::writeReg16Bit(uint8_t reg, uint16_t value, uint8_t i2c_address)
{
  //Write order is LSB first, and then MSB. Refer to AN635 pg 35 figure 1.12.2.5
  Wire.beginTransmission(i2c_address);
  Wire.write(reg);
  Wire.write( value       & 0xFF); // value low byte
  Wire.write((value >> 8) & 0xFF); // value high byte
  Wire.endTransmission();
}

bool MAX17055TwoWireSerif::writeVerifyReg16Bit(uint8_t reg, uint16_t value, uint8_t i2c_address)
{
  int attempt = 0;
  // Verify that the value has been written before moving on
  while ((value != readReg16Bit(reg, i2c_address)) && (attempt < 10)) {
    std::cout << "    Resetting Status ... attempt " << attempt << std::endl;
    //Write the value to the register
    writeReg16Bit(reg, value, i2c_address);
    // Wait a bit
    sygsp::delay(1);

    //Increase attempt
    attempt++;
  };
  
  if (attempt > 10) {
    return false;
    std::cout << "    Failed to write value" <<std::endl;
  } else {
    std::cout << "    Value successfully written" << std::endl;
    return true;
  }
}

}

// *explicit external instantiations*
extern template struct MAX17055TwoWireSerif<0x36>;

} }
