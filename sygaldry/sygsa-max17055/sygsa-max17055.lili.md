\page page-sygsa-max17055 MAX17055

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This is a simple driver for the MAX17055 fuel gauge. The fuel gauge is controlled via its control registers over the I2C bus. This driver was adapted from the driver found at https://github.com/AwotG/Arduino-MAX17055_Driver.

This library will be structured using a helpers file that has functions for reading and writing to the registers, and a helpers file that stores a dictionary of the registers.

# Helpers

## I2C Interface

We define three functions for reading data from the I2C interface: readReg16Bit, writeReg16Bit, and writeverifyReg16Bit.


```cpp
// @#'sygsa-max17055-helpers.hpp'
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
// @/
```
The functions simply read/write from the provided registers
```cpp
// @#'sygsa-max17055-helpers.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsa-max17055-helpers.hpp"
#include "sygsp-delay.hpp"
#include <Wire.h>
namespace sygaldry { namespace sygsa {
uint8_t readReg16Bit(uint8_t i2c_address, uint8_t reg)
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

static void writeReg16Bit(uint8_t i2c_address, uint8_t reg, uint16_t value)
{
  //Write order is LSB first, and then MSB. Refer to AN635 pg 35 figure 1.12.2.5
  Wire.beginTransmission(i2c_address);
  Wire.write(reg);
  Wire.write( value       & 0xFF); // value low byte
  Wire.write((value >> 8) & 0xFF); // value high byte
  Wire.endTransmission();
}

static bool writeVerifyReg16Bit(uint8_t i2c_address, uint8_t reg, uint16_t value)
{
  int attempt = 0;
  // Verify that the value has been written before moving on
  while ((value != readReg16Bit(reg, i2c_address)) && (attempt < 10)) {
    std::cout << "    Resetting Status ... attempt " << attempt << std::endl;
    //Write the value to the register
    writeReg16Bit(reg, value, i2c_address);
    // Wait a bit
    delay(1);

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

}
```

## Registers
Registers and values for the Least Significant Bit (LSB) are stored in a .hpp file.

```cpp
// @#'sygsa-max17055-registers.hpp'
// Define registers for the MAX17055 fuel gauge
enum regAddr
{
STATUS_REG      = 0x00, //Maintains all flags related to alert thresholds and battery insertion or removal.
AGE_REG         = 0x07, //calculated percentage value of capacity compared to original design capacity.
TEMP_REG        = 0x08, //Temperature of MAX17055 chip
VCELL_REG       = 0x09, //VCell reports the voltage measured between BATT and CSP.
AVGVCELL_REG    = 0x19, //The AvgVCell register reports an average of the VCell register readings. 
CURRENT_REG     = 0x0A, //Voltage between the CSP and CSN pins, and would need to convert to current
AVGCURRENT_REG  = 0x0B, //The AvgCurrent register reports an average of Current register readings
REPSOC_REG      = 0x06, //The Reported State of Charge of connected battery.
ICHTERM_REG     = 0x1E, // Register fo setting end of charge current 
REPCAP_REG      = 0x05, //Reported Capacity.
TTE_REG         = 0x11, //How long before battery is empty (in ms).
TTF_REG         = 0x20, //How long until the battery is full (in ms)
DESIGNCAP_REG   = 0x18, //Capacity of battery inserted, not typically used for user requested capacity
VEMPTY_REG      = 0x3A, //Register for voltage when the battery is empty
dQACC_REG       = 0x45, //Register for dQAcc
dPACC_REG       = 0x46, //Register for dPAcc
MODELCFG_REG    = 0xDB, // Register for MODELCFG
RCOMPP0_REG     = 0x38, // Register for learned parameter rcomp0, open circuit voltage characterisation
TEMPCO_REG      = 0x39, // Register for learned parameter tempco, temperature compensation information
FULLCAP_REG     = 0x10, // Register for learned parameter full capacity
CYCLES_REG      = 0x17, // Register for learned parameter charge cycles
FULLCAPNORM_REG = 0x23, // Register for learned parameter full capacity (normalised)
};

//Based on "Register Resolutions from MAX17055 Technical Reference" Table 6. 
float base_capacity_multiplier_mAh = 5.0f; // base capacity multiplier divide by rsense(mOhms) to get LSB
float base_current_multiplier_mAh = 1.5625f; // base current multiplier divide by rsense(mOhms) to get LSB
float voltage_multiplier_V = 7.8125e-5; //refer to row "Voltage"
float time_multiplier_Hours = 5.625f/3600.0f; //Least Significant Bit= 5.625 seconds, 3600 converts it to Hours.
float percentage_multiplier = 1.0f/256.0f; //refer to row "Percentage"
```

# Component

To initialise the MAX17055 component we need several inputs. These include the


```cpp
// @#'sygsa-max17055.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

// include statements

namespace sygaldry { namespace sensors { namespace esp32 {

/*! \addtogroup sygXY
*/
/// \{

/*! \defgroup sygXY-newcomponent New Component
*/
/// \{

/*! \brief brief doc

detailed doc
*/
struct NewComponent
: name_<"New Component">
, description_<"Description goes here">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct inputs_t {
    } inputs;

    struct outputs_t {
    } outputs;

    /// brief doc
    void init();

    /// brief doc
    void main();
}

/// \}
/// \}

} } }
// @/
```

```cpp
// @#'sygXY-newcomponent.cpp'
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygXY-newcomponent.hpp"

namespace sygaldry { namespace sygXY {

void NewComponent::init()
{
}

void NewComponent::main()
{
}

} } }
// @/
```

```cpp
// @#'sygXY-newcomponent.test.cpp'
#include <catch2/catch_test_macros.hpp>
#include "sygXY-newcomponent.hpp"

using namespace sygaldry;
using namespace sygaldry::sygXY;

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygXY-newcomponent)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_include_directories(${lib} PUBLIC .)
target_link_libraries(${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygaldry-helpers-metadata
        )

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
#target_link_libraries(${lib}-test PRIVATE OTHERREQUIREDPACKAGESANDCOMPONENTSHERE)
catch_discover_tests(${lib}-test)
endif()
# @/
```
