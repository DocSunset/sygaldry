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
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sygah-metadata.hpp"
#include "sygse-max17055-tests.hpp"
#include "sygsp-delay.hpp"
#include "sygsp-micros.hpp"
#include "sygah-endpoints.hpp"
#include "sygsa-max17055-registers.hpp"
#include "sygsa-max17055-helpers.hpp"
#include "Wire.h"

namespace sygaldry { namespace sygsa {

struct MAX17055
: name_<"MAX17055 Fuel Gauge">
, description_<"Simple driver for MAX17055 fuel gauge">
, author_<"Albert Niyonsenga">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct inputs_t {
        // Initialisation Elements
        uint8_t i2c_addr; // i2c address of the fuel guage
        int designcap; // design capacity of the batteries in mAh
        int ichg; // Charge termination current in mA
        float rsense; // Resistance of current sense resistor (mOhm))
        float vempty; // Empty voltage of the battery (V)
        float recovery_voltage; // Recovery voltage of the battery (V)

        // Learned Parameters
        int soc; // raw State of charge (5mVh/rsense)
        int rcomp; // compensation parameter for battery
        int tempco; // temperature compensation parameter
        int fullcap; // full capacity of battery (5mVh/rsense)
        int fullcapnorm; // full capacity of battery normalised
        int cycles; // charge cycles of the battery

        // Other parameters
        int pollrate; // poll rate in seconds
    } inputs;

    struct outputs_t {
        // ANALOG MEASUREMENTS
        // Current
        slider<"raw instantaneous current", "LSB", int, -32768, 32767, 0> inst_curr_raw;
        slider<"raw average current", "LSB", int, -32768, 32767, 0> avg_curr_raw;
        slider<"instantaneous current", "mA", float, -5.12f, 5.12f, 0.0f> inst_curr;
        slider<"average current", "mA", float, -5.12f, 5.12f, 0.0f> avg_curr;
        // Voltage
        slider<"raw instantaneous voltage", "LSB", int, 0, 65535, 0> inst_voltage_raw;
        slider<"raw average voltage", "LSB", int, 0, 65535, 0> avg_voltage_raw;
        slider<"instantaneous voltage", "V", float, 0.0f, 5.11992f, 0.0f> inst_voltage;
        slider<"average voltage", "V", float, 0.0f, 5.11992f, 0.0f> avg_voltage;

        // MODEL OUTPUTS
        // Capacity
        slider<"raw capacity", "LSB", int, 0, 65535, 0> capacity_raw;
        slider<"raw full capacity", "LSB", int, 0, 65535, 0> fullcapacity_raw;
        slider<"capacity", "mAh", int, 0, 32000, 0> capacity;
        slider<"full capacity", "mAh", int, 0, 32000, 0> fullcapacity;
        // Capacity (norm)
        slider<"raw full capacity normalised", "LSB", int, 0, 65535, 0> fullcapacitynorm_raw;
        slider<"full capacity", "%", float, 0.0f, 255.9961f, 0.0f> fullcapacitynorm;
        // SOC, Age
        slider<"raw state of charge", "LSB", int, 0, 65535, 0> soc_raw; // LSB
        slider<"raw battery age", "LSB",  int, 0, 65535, 0> age_raw; // LSB
        slider<"state of charge", "%", float, 0.0f, 255.9961f, 0.0f> soc; // percentage
        slider<"battery age", "%", float, 0.0f, 255.9961f, 0.0f> age; // percentage
        // Time to full (TTF), Time to empty (TTE), age
        slider<"raw Time to full", "LSB", int, 0, 65535, 0> ttf_raw; // LSB
        slider<"raw Time to empty", "LSB", int, 0, 65535, 0> tte_raw; // LSB
        slider<"rime to full", "h", float, 0.0f, 102.3984f, 0.0f> ttf; // hours
        slider<"time to empty", "h", float, 0.0f, 102.3984f, 0.0f> tte;  // hours
        // Cycles
        slider<"raw charge cycles", "LSB", int, 0, 65535, 0> chargecyles_raw;
        slider<"charge cycles", "num", float, 0.0f, 655.35f, 0.0f> chargecyles;
        // Parameters
        slider<"rcomp", "LSB", int, 0, 65535, 0> rcomp;
        slider<"tempco", "LSB", int, 0, 65535, 0> tempco;

        // Battery Status
        toggle<"present"> status;
        toggle<"removed"> removed;
        toggle<"inserted"> inserted;

        text_message<"error message"> error_message;

        toggle<"running"> running;
    } outputs;

    /// initialize the MAX17055 for continuous reading
    void init()
    {
        outputs.running = true;
        uint16_t STATUS = readReg16Bit(inputs.i2c_addr,STATUS_REG);
        uint16_t POR = STATUS&0x0002;
        std::cout << "    Checking status " << "\n"
                << "    Status read: " << STATUS << "\n"
                << "    POR flag: " << POR << std::endl;

        // Reset the Fuel Gauge
        if (POR)
        {
            std::cout << "    Initialising Fuel Gauge" << std::endl;
            while(readReg16Bit(inputs.i2c_addr, 0x3D)&1) {
                sygsp::delay(10);
            }

            std::cout << "    Start up complete" << std::endl;
            //Initialise Configuration
            uint16_t HibCFG = readReg16Bit(inputs.i2c_addr, 0xBA);
            // Exit hibernate mode
            writeReg16Bit(inputs.i2c_addr, 0x60, 0x90);
            writeReg16Bit(inputs.i2c_addr, 0xBA, 0x0);
            writeReg16Bit(inputs.i2c_addr, 0x60, 0x0);

            //EZ Config
            // Write Battery capacity
            std::cout << "    Writing Capacity" << std::endl;
            uint16_t reg_cap = (inputs.designcap * inputs.rsense) / base_capacity_multiplier_mAh;
            uint16_t reg_ichg = (inputs.ichg * inputs.rsense) / base_current_multiplier_mAh;
            writeReg16Bit(inputs.i2c_addr, DESIGNCAP_REG, reg_cap); //Write Design Cap
            writeReg16Bit(inputs.i2c_addr, ICHTERM_REG, reg_ichg); // End of charge current
            writeReg16Bit(inputs.i2c_addr, dQACC_REG, reg_cap/32); //Write dQAcc
            writeReg16Bit(inputs.i2c_addr, dPACC_REG, 44138/32); //Write dPAcc

            // Set empty voltage and recovery voltage
            // Empty voltage in increments of 10mV
            std::cout << "    Writing Voltage" << std::endl;
            uint16_t reg_vempty = inputs.vempty * 100; //empty voltage in 10mV
            uint16_t reg_recover = 3.88 *25; //recovery voltage in 40mV increments
            uint16_t voltage_settings = (reg_vempty << 7) | reg_recover; 
            writeReg16Bit(inputs.i2c_addr, VEMPTY_REG, voltage_settings); //Write Vempty
            
            // Set Model Characteristic
            writeReg16Bit(inputs.i2c_addr, MODELCFG_REG, 0x8000); //Write ModelCFG

            //Wait until model refresh
            while(readReg16Bit(inputs.i2c_addr, MODELCFG_REG)&0x8000) {
                sygsp::delay(10);
            }
            //Reload original HbCFG value
            writeReg16Bit(inputs.i2c_addr, 0xBA,HibCFG);    
        } else {
            std::cout << "    Loading old config" << std::endl;
        }
        // Reset Status Register when init function runs
        std::cout << "    Resetting Status" << std::endl;
        STATUS = readReg16Bit(inputs.i2c_addr,STATUS_REG);
        
        // Get new status
        uint16_t RESET_STATUS = STATUS&0xFFFD;
        std::cout << "    Setting new status: " << RESET_STATUS << std::endl;
        writeVerifyReg16Bit(inputs.i2c_addr,STATUS_REG,RESET_STATUS); //reset POR Status   

        // Read Status to ensure it has been cleared (for debugging)
        POR = readReg16Bit(inputs.i2c_addr,STATUS_REG)&0x0002;
        std::cout << "    Status Flag: " << readReg16Bit(inputs.i2c_addr,STATUS_REG) << "\n"
                << "    POR Flag: " << POR << std::endl;     
    }

    // poll the MAX17055 for new data and update endpoints
    void main()
    {
        if (!outputs.running) return; // TODO: 
        static auto prev = sygsp::micros();
        auto now = sygsp::micros();
        if (now-prev > (inputs.pollrate*1e6)) {
            prev = now;
            // CCompute capacity and current multipliers
            float curr_multiplier = base_current_multiplier_mAh / inputs.rsense;
            float cap_multiplier = base_capacity_multiplier_mAh /  inputs.rsense;
            // ANALOG MEASUREMENTS
            // Current
            outputs.inst_curr_raw = readReg16Bit(inputs.i2c_addr, CURRENT_REG);
            outputs.avg_curr_raw = readReg16Bit(inputs.i2c_addr, AVGCURRENT_REG);
            outputs.inst_curr = curr_multiplier * outputs.inst_curr;
            outputs.avg_curr = curr_multiplier * outputs.avg_curr;
            // Voltage
            outputs.inst_voltage_raw = readReg16Bit(inputs.i2c_addr, VCELL_REG);
            outputs.avg_voltage_raw = readReg16Bit(inputs.i2c_addr, AVGVCELL_REG);
            outputs.inst_voltage = voltage_multiplier_V * outputs.inst_voltage_raw;
            outputs.avg_voltage = voltage_multiplier_V * outputs.avg_voltage_raw;
            // MODEL OUTPUTS
            // Capacity
            outputs.capacity_raw = readReg16Bit(inputs.i2c_addr, REPCAP_REG);
            outputs.fullcapacity_raw = readReg16Bit(inputs.i2c_addr, FULLCAP_REG);
            outputs.fullcapacitynorm_raw = readReg16Bit(inputs.i2c_addr, FULLCAPNORM_REG);
            outputs.capacity = cap_multiplier * outputs.capacity_raw;
            outputs.fullcapacity = cap_multiplier * outputs.fullcapacity_raw;
            outputs.fullcapacitynorm = percentage_multiplier * outputs.fullcapacitynorm_raw;
            // SOC, Age
            outputs.age_raw = readReg16Bit(inputs.i2c_addr, AGE_REG);
            outputs.soc_raw = readReg16Bit(inputs.i2c_addr,REPSOC_REG);
            outputs.age = percentage_multiplier * outputs.age_raw;
            outputs.soc = percentage_multiplier * outputs.soc_raw;
            // TTF,TTE
            outputs.tte_raw = readReg16Bit(inputs.i2c_addr, TTE_REG);
            outputs.ttf_raw = readReg16Bit(inputs.i2c_addr, TTF_REG);
            outputs.tte = time_multiplier_Hours * outputs.tte_raw;
            outputs.ttf = time_multiplier_Hours * outputs.ttf_raw;
            // Cycles
            outputs.chargecyles_raw = readReg16Bit(inputs.i2c_addr, CYCLES_REG);
            outputs.chargecyles = 0.01f * outputs.chargecyles_raw;
            // Parameters
            outputs.rcomp =  readReg16Bit(inputs.i2c_addr, RCOMPP0_REG);
            outputs.tempco = readReg16Bit(inputs.i2c_addr, TEMPCO_REG);

            // Read battery status
            uint16_t raw_status = readReg16Bit(inputs.i2c_addr,STATUS_REG);

            // Get the 4th bit
            bool bat_status = raw_status&0x0800;
            outputs.status = !bat_status; // battery status 0 when present, must invert
            // Get insertion
            outputs.inserted = raw_status&0x0800; // Get the 11th bit
            // Get removed
            outputs.removed = raw_status&0x8000;  // get the 15th bit
            
            // Reset Insertion bit
            writeVerifyReg16Bit(inputs.i2c_addr, STATUS_REG, raw_status&0xF7F);
            // Reset Removal bit
            writeVerifyReg16Bit(inputs.i2c_addr, STATUS_REG, raw_status&0x7FFF);
        }
    }
};

} }
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
