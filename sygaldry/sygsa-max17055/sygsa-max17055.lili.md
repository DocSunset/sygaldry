\page page-sygsa-max17055 MAX17055

Copyright 2023 Albert-Ngabo Niyonsenga, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This library is a simple driver for the MAX17055 fuel gauge. The fuel gauge is controlled via its control registers over the I2C bus. This driver was adapted from the driver found at https://github.com/AwotG/Arduino-MAX17055_Driver and from the implementation in the T-Stick firmware found at https://github.com/aburt2/T-Stick/tree/pcb_design. 

This library will be structured using a helpers file that stores a dictionary of the registers and the multipliers to compute the Least Significant Bit for the capacity, current, voltage, percentage and time registers.

# Helpers

Registers and values for the Least Significant Bit (LSB) are stored in a .hpp file. 
```cpp
// @#'sygsa-max17055-helpers.hpp'
/*
Copyright 2023 Albert-Ngabo Niyonsenga Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada

SPDX-License-Identifier: MIT
*/
#pragma once

// Define registers for the MAX17055 fuel gauge
enum regAddr
{
STATUS_REG      = 0x00, ///< Maintains all flags related to alert thresholds and battery insertion or removal.
AGE_REG         = 0x07, ///< calculated percentage value of capacity compared to original design capacity.
TEMP_REG        = 0x08, ///< Temperature of MAX17055 chip
VCELL_REG       = 0x09, ///< VCell reports the voltage measured between BATT and CSP.
AVGVCELL_REG    = 0x19, ///< The AvgVCell register reports an average of the VCell register readings. 
CURRENT_REG     = 0x0A, ///< Voltage between the CSP and CSN pins, and would need to convert to current
AVGCURRENT_REG  = 0x0B, ///< The AvgCurrent register reports an average of Current register readings
REPSOC_REG      = 0x06, ///< The Reported State of Charge of connected battery.
ICHTERM_REG     = 0x1E, ///< Register fo setting end of charge current 
REPCAP_REG      = 0x05, ///< Reported Capacity.
TTE_REG         = 0x11, ///< How long before battery is empty (in ms).
TTF_REG         = 0x20, ///< How long until the battery is full (in ms)
DESIGNCAP_REG   = 0x18, ///< Capacity of battery inserted, not typically used for user requested capacity
VEMPTY_REG      = 0x3A, ///< Register for voltage when the battery is empty
dQACC_REG       = 0x45, ///< Register for dQAcc
dPACC_REG       = 0x46, ///< Register for dPAcc
MODELCFG_REG    = 0xDB, ///< Register for MODELCFG
RCOMPP0_REG     = 0x38, ///< Register for learned parameter rcomp0, open circuit voltage characterisation
TEMPCO_REG      = 0x39, ///< Register for learned parameter tempco, temperature compensation information
FULLCAP_REG     = 0x10, ///< Register for learned parameter full capacity
CYCLES_REG      = 0x17, ///< Register for learned parameter charge cycles
FULLCAPNORM_REG = 0x23, ///< Register for learned parameter full capacity (normalised)
};

// Set Fuel Gauge I2C address
static constexpr int i2c_addr = 0x36; ///< set fuel gauge i2c address as it is a constant

//Based on "Register Resolutions" from MAX17055 Technical Reference Table 6. 
static constexpr float base_capacity_multiplier_mAh = 5.0f; ///< base capacity multiplier divide by rsense(mOhms) to get LSB
static constexpr float base_current_multiplier_mAh = 1.5625f; ///< base current multiplier divide by rsense(mOhms) to get LSB
static constexpr float voltage_multiplier_V = 7.8125e-5; ///< refer to row "Voltage"
static constexpr float time_multiplier_Hours = 5.625f/3600.0f; ///< Least Significant Bit= 5.625 seconds, 3600 converts it to Hours.
static constexpr float percentage_multiplier = 1.0f/256.0f; ///< refer to row "Percentage"

// @/
```

In addition to the registers and multiplier information we also store useful defaults for battery operation that should be okay for most usecases. We do this as the defaults in the MAX17055 fuel gauge especially the empty voltage and the end of charge current are not appropriate for most use cases using a single cell Lithium-Ion/LiPo with a microcontroller.

```cpp
//@+'sygsa-max17055-helpers.hpp'

// Default configuration
static constexpr int default_capacity = 2600; ///< Default battery capacity (mAh)
static constexpr int default_ichg = 50; ///< Default end of charge current, typical single cell linear charger end of charge current (mA)
static constexpr int default_rsense = 10; ///< Default sense resistor value, decent sense resistor value
static constexpr int default_vempty = 3; ///< Default empty voltage, good for 3.3V devices with low drop out LDO
static constexpr int default_recovery_voltage = 3.8; ///< Recommened default by Analog Devices
static constexpr int default_poll_rate = 60000; ///< Default poll rate of 60s

// @/
```

# Component

To initialise the MAX17055 component we have several inputs. These are stored as sliders with explicit maximums and minimums as well as default values. 

Capacity and current rsolution is dependent on the current sensors resistance. As stated by the MAX17055 User Guide in Table 1.3. The resolutions are given below. Note that this means that for the capacity and current a high resistance sense resistor, lowers both the resolution and the maximum capacity/current values that the fuel gauge can measure.

| **Register Type** | **LSB Size**     |
|-------------------|------------------|
| Capacity          | 5.0 mVh/rsense   |
| Current           | 1.5625 mV/rsense |
| Voltage           | 7.8125e-5 V      |
| Percentage        | 1/256 %          |
| Time              | 5.625/3600 h     |

```cpp
// @#'sygsa-max17055.hpp'
/*
Copyright 2023 Albert-Ngabo Niyonsenga Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada

SPDX-License-Identifier: MIT
*/
#pragma once
#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"
#include "sygsa-max17055-helpers.hpp"

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
        // Fuel gauge inputs
        slider_message<"capacity", "Design capacity of the battery (mAh)", int, 0, 32000, 0, tag_session_data> designcap;
        slider_message<"current sense resistor", "Resistance of current sense resistor (mOhm))", int, 0, 100, 10, tag_session_data> rsense; // sense resistor values above 100Ohms lead to poor capacity and current resolution
        slider_message<"poll rate", "Fuel gauge poll rate (ms)", int, 10000, 300000, 60000, tag_session_data> pollrate; // should not poll fuel gauge to quickly
        slider_message<"end-of-charge current", "End of charge current (mA)", int,  0, 300, 50, tag_session_data> ichg;
        slider_message<"Empty Voltage", "Empty voltage of the battery (V)", float, 0.0f, 4.2f, 3.0f, tag_session_data>  vempty; 
        slider_message<"Recovery voltage", "Recovery voltage of the battery (V)", float, 0.0f, 4.2f, 3.8f, tag_session_data> recovery_voltage;

        // Restart policy parameters
        @{restart-inputs}

    } inputs;

    struct outputs_t {
        // ANALOG MEASUREMENTS
        // Current
        slider<"instantaneous current", "mA", float> inst_curr;
        slider<"average current", "mA", float> avg_curr;
        // Voltage
        slider<"instantaneous voltage", "V", float, 0.0f, 5.11992f, 0.0f> inst_voltage;
        slider<"average voltage", "V", float, 0.0f, 5.11992f, 0.0f> avg_voltage;

        // MODEL OUTPUTS
        // Capacity
        slider<"raw full capacity", "full capacity of the battery", int, 0, 65535, 0, tag_session_data> fullcapacity_raw; // maximum raw value for 16 bit integer
        slider<"capacity", "current capacity (mAh)", int> capacity;
        slider<"full capacity", "full capacity of the battery (mAh)", int> fullcapacity;
        // Capacity (nom)
        slider<"raw full capacity nominal", "full capacity of the battery (no voltage/temperature compensation)", int, 0, 65535, 0, tag_session_data> fullcapacitynom_raw; // maximum raw value for 16 bit integer
        // SOC, Age
        slider<"state of charge", "%", float, 0.0f, 255.9961f, 0.0f> soc; // percentage
        slider<"battery age", "full battery capacity divided by design capacity (%)", float, 0.0f, 255.9961f, 0.0f> age; // percentage
        // Time to full (TTF), Time to empty (TTE), age
        slider<"time to full", "h", float, 0.0f, 102.3984f, 0.0f> ttf; // hours
        slider<"time to empty", "h", float, 0.0f, 102.3984f, 0.0f> tte;  // hours
        // Cycles
        slider<"raw charge cycles", "LSB", int, 0, 65535, 0, tag_session_data> chargecycles_raw; // maximum raw value for 16 bit integer
        slider<"charge cycles", "number of charge cycles", float, 0.0f, 655.35f, 0.0f> chargecycles;
        // Parameters
        slider<"rcomp", "voltage compensation parameter", int, 0, 65535, 0, tag_session_data> rcomp; // will change as the battery ages, should be stored for parameter restoration
        slider<"tempco", "temperature compensation parameter", int, 0, 65535, 0, tag_session_data> tempco; // will change as the battery ages, should be stored for parameter restoration

        // Battery Status
        toggle<"present", "Shows if battery is present"> status;

        // Error and status messages
        text_message<"error message", "Error message from fuel gauge"> error_message;
        text_message<"status message", "Status message from fuel gauge"> status_message;

        @{restart-outputs}
    } outputs;

    // initialize the MAX17055 for continuous reading
    void init();

    // poll the MAX17055 for new data and update endpoints
    void main();

    // restart MAX17055
    void restart();

    // Read 16 bit register
    uint16_t readReg16Bit(uint8_t reg);
    
    // Write to 16 bit register
    void writeReg16Bit(uint8_t reg, uint16_t valu);
    
    // Write and verify to 16 bit register
    bool writeVerifyReg16Bit(uint8_t reg, uint16_t value);

    // Write design capacity
    bool writeDesignCapacity();

    // Write end of charge current
    bool writeICHG();

    // Write Vempty and recovery voltage
    bool writeVoltage();

    // Restore old parameters
    bool restoreParameters();
};

} }
// @/
```

Below is the full implementation of the init and main subroutines, as well as functions for reading and writing to the registers of the MAX17055

```cpp
// @#'sygsa-max17055.impl.hpp'
/*
Copyright 2023 Albert-Ngabo Niyonsenga Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada

SPDX-License-Identifier: MIT
*/
#pragma once
#include "Arduino.h"
#include "sygsp-micros.hpp"
#include "sygsa-max17055-helpers.hpp"
#include <iostream>
#include "Wire.h"
#include "sygsa-max17055.hpp"
#include "sygsp-restart-agent.hpp"

namespace sygaldry { namespace sygsa {
    /// initialize the MAX17055 for continuous reading
    void MAX17055::init()
    {
        @{init}
    }

    // poll the MAX17055 for new data and update endpoints
    void MAX17055::main()
    {
        @{main}
    }

    // restart the MAX17055 fuel gauges
    void MAX17055::restart() {
        @{restart}
    }

    @{wire}

    @{helpers}
}
} 
// @/
```

## Reading and Writing to Registers
Reading and writing to the registers use pretty standard application of read/write. There is also an additiona write and verify function recommended by the MAX17055 Software guide that verifies that the value has been written to the register. This is done for critical values like Status flags that need to be reset.

```cpp
//@='wire'
/// Read 16 bit register
uint16_t MAX17055::readReg16Bit(uint8_t reg)
{
    uint16_t value = 0;  
    Wire.beginTransmission(i2c_addr); 
    Wire.write(reg);
    Wire.endTransmission(false);
    
    Wire.requestFrom(i2c_addr, (uint8_t) 2); 
    value  = Wire.read();
    value |= (uint16_t)Wire.read() << 8;      // value low byte
    return value;
}

/// Write to 16 bit register
void MAX17055::writeReg16Bit(uint8_t reg, uint16_t value)
{
    //Write order is LSB first, and then MSB. Refer to AN635 pg 35 figure 1.12.2.5
    Wire.beginTransmission(i2c_addr);
    Wire.write(reg);
    Wire.write( value       & 0xFF); // value low byte
    Wire.write((value >> 8) & 0xFF); // value high byte
    Wire.endTransmission();
}

/// Write and verify to 16 bit register
bool MAX17055::writeVerifyReg16Bit(uint8_t reg, uint16_t value)
{
    int attempt = 0;
    // Verify that the value has been written before moving on
    while ((value != readReg16Bit(reg)) && (attempt < 10)) {
        //Write the value to the register
        writeReg16Bit(reg, value);
        // Wait a bit
        delay(1);

        //Increase attempt
        attempt++;
    };
    
    if (attempt > 10) {
        return false;
        outputs.error_message = "Failed to write value";
    } else {
        outputs.status_message = "Value successfully written";
        return true;
    }
}
// @/
```

## Helper Functions
In addition to functions for reading and writing to registers. Functions for writing specific properties to the fuel gauge were also written. This allows us to update the properties from the command line without having to restart the device.

In addition the restore parameter function restores the ModelGauge parameters after a power loss event. The MAX17055 slowly learns and adjust its state of charge prediction with time based on voltage, current and temperature information. This is useful as it helps the fuel gauge become more accurate over time and account for changes in battery chemistry as the battery ages. However, if the fuel gauge is power cycled or it receives a software reset signal, we have to restore the old parameters. Power cycling events are not well described by Analog Device's documentation but seem to occur when the battery completely dies (ie: it goes below 3V) and not during regular operations such as unplugging the battery or having the device be asleep.

After a power loss or major software change the fuelgauge IC resets, in order to retain past information from the battery (charge cycles, full capacity, etc.) they must be restored manually from previously stored values. 

```cpp
//@='helpers'
// helper functions for reading properties
/// Write design capacity
bool MAX17055::writeDesignCapacity() {
    uint16_t reg_cap = (inputs.designcap * inputs.rsense) / base_capacity_multiplier_mAh;
    if (!writeVerifyReg16Bit(DESIGNCAP_REG, reg_cap)) {
        return false;
    } //Write Design Cap
    if (!writeVerifyReg16Bit(dQACC_REG, reg_cap/32)) {
        return false;
    } //Write dQAcc
    if (!writeVerifyReg16Bit(dPACC_REG, 44138/32)) {
        return false;
    } //Write dPAcc
    return true;
};

/// Write end of charge current
bool MAX17055::writeICHG() {
    uint16_t reg_ichg = (inputs.ichg * inputs.rsense) / base_current_multiplier_mAh;
    return writeVerifyReg16Bit(ICHTERM_REG, reg_ichg);
};

/// Write Vempty and recovery voltage
bool MAX17055::writeVoltage() {
    uint16_t reg_vempty = inputs.vempty * 100; //empty voltage in 10mV
    uint16_t reg_recover = 3.88 *25; //recovery voltage in 40mV increments
    uint16_t voltage_settings = (reg_vempty << 7) | reg_recover; 
    return writeVerifyReg16Bit(VEMPTY_REG, voltage_settings); //Write Vempty session_data
};

/// Restore old parameters
bool MAX17055::restoreParameters() {
    // Output status message
    outputs.status_message = "Restoring old parameters";

    // Write nominal full capacity, rcomp and tempco
    if (!writeVerifyReg16Bit(TEMPCO_REG, outputs.tempco)) {
        return false;
    };
    if (!writeVerifyReg16Bit(RCOMPP0_REG, outputs.rcomp)) {
        return false;
    };
    if (!writeVerifyReg16Bit(FULLCAPNORM_REG, outputs.fullcapacitynom_raw)) {
        return false;
    }
    ;

    // Delay from 350ms
    delay(350);

    // Write calculated remaining capacity and percentage of cell
    outputs.fullcapacitynom_raw = readReg16Bit(FULLCAPNORM_REG);
    uint16_t mixcap = (readReg16Bit(0x0D)*outputs.fullcapacitynom_raw) / 25600;
    if (!writeVerifyReg16Bit(0x0F,mixcap)) {
        return false;
    } 
    if (!writeVerifyReg16Bit(FULLCAP_REG, outputs.fullcapacity_raw)) {
        return false;
    }

    // Set dQacc to 200% of capacity and dPacc to 200%
    writeReg16Bit(dQACC_REG, outputs.fullcapacity_raw / 16); //Write dQAcc
    writeReg16Bit(dPACC_REG, 0x0C80); //Write dQAcc

    // Delay for 350ms
    delay(350);

    // Restore cycles
    if (!writeVerifyReg16Bit(CYCLES_REG, outputs.chargecycles_raw)) {
        return false;
    }

    // Return true when finished
    return true;
};
// @/
```

## Using the Restart Agent
Occasionally temporary errors, may cause the component to not respond and the component to set to not running. We want to be able to set a restart policy for the component so that the system can try to ping the component again to see if it responds. We use the `sygsp-restart-agent` component to handle the restart logic. All we need to do is to make sure to initialise it in the init routine and have the appropriate inputs and outputs and define a restart function.

```cpp
//@='restart-inputs'
slider_message<"restart policy","Set the restart policy for the component", int, 1, 4, 1, tag_session_data> restart_policy;
slider_message<"restart attempts","Set the max amount of restart attempts", int,0, 10, 0, tag_session_data> max_attempts;
toggle<"stop signal", "Indicate that the fuel gauge should stop running", 0, tag_session_data> stop_signal;
toggle<"attempt restart", "Indicates if the fuel gauge attempts to restart when it failed."> attempt_restart;
slider_message<"restart time","Set the time between restart attempts", int,  5000, 30000, 5000, tag_session_data> restart_time;
// @/
```

```cpp
//@='restart-outputs'
slider_message<"current attempt", "Current attempt for restarting fuel guage"> curr_attempt; // Current restart attempt
toggle<"running", "Indicate if fuel gauge is running"> running;
// @/
```

For the restart function we just call the init routine of the MAX17055 fuel gauge again. Most errors of the fuel gauge are caused by failure to write to a configuration register, so we redo the init function to be safe.

```cpp
//@='restart'
init();
// @/
```

## Init Subroutine
The init subroutine applies the EZConfig implementation shown in MAX17055 Software Implementation Guide. The status register is read to check if a hardware/osftware event occured if it did then the fuel gauge must be initiliased.

```cpp
//@='init'
// Initialise restart agent
sygsp::RestartAgent agent;
agent.init();

// Set the inputs 
if (inputs.designcap != 0) {
    inputs.designcap = default_capacity;
    inputs.rsense = default_ichg;
    inputs.pollrate = default_poll_rate;
    inputs.ichg = default_ichg;
    inputs.vempty = default_vempty;
    inputs.recovery_voltage = default_recovery_voltage;
}

// Configure restart agent
agent.configureComponent(&this);

// Read the status registry and check for hardware/software reset
uint16_t STATUS = readReg16Bit(STATUS_REG);
uint16_t POR = STATUS&0x0002;
// @/
```

When resetting the fuel gauge we initially make sure to make sure the fuel gauge is out of hibernation mode

```cpp
//@+'init'
// Initialise the restart agent

// Reset the Fuel Gauge
if (POR)
{
    while(readReg16Bit(0x3D)&1) {
        delay(10);
    }

    //Initialise Configuration
    uint16_t HibCFG = readReg16Bit(0xBA);
    // Exit hibernate mode
    writeReg16Bit(0x60, 0x90);
    writeReg16Bit(0xBA, 0x0);
    writeReg16Bit(0x60, 0x0);
// @/
```
The design capacity, empty voltage, recovery voltage, and end of charge current are converted to 16 bit integers to be stored in the registers of the MAX17055. 

```cpp
//@+'init'
   //EZ Config
    // Write Battery capacity
    outputs.status_message = "Set design capacity and current for fuel gauge";
    if (!writeDesignCapacity()) {
        outputs.running = false;
        outputs.error_message = "Failed to write design capacity, disabling fuel gauge";
        return; // 
    } //Write Design Cap
    if (!writeICHG()) {
        outputs.running = false;
        outputs.error_message = "Failed to write end of charge current, disabling fuel gauge";
        return; // 
    } // End of charge current

    // Set empty voltage and recovery voltage
    // Empty voltage in increments of 10mV
    if (!writeVoltage()) {
        outputs.running = false;
        outputs.error_message = "Failed to recovery and empty voltage, disabling fuel gauge";
        return; //         
    }
// @/
```

Once the values have been written, Status flag is reset to prepare for a new hardware event. We use write and verify to ensure the status flag is reset. We output an error message if the status flag cannot be reset.

```cpp
//@+'init'
    // Set Model Characteristic
    if (!writeVerifyReg16Bit(MODELCFG_REG, 0x8000)) {
        outputs.running = false;
        outputs.error_message = "Failed to write new model, disabling fuel gauge";
        return; //         
    }; //Write ModelCFG

    //Wait until model refresh
    outputs.status_message = "Waiting for model refresh";
    while(readReg16Bit(MODELCFG_REG)&0x8000) {
        delay(10);
    }
    outputs.status_message = "Model refresh complete";
    //Reload original HbCFG value
    if (!writeVerifyReg16Bit(0xBA,HibCFG)) {
        outputs.running = false;
        outputs.error_message = "Failed to set hibernation config";
        return;
    }

    // Restore old parameters
    if (outputs.fullcapacitynom_raw != 0) {
        if (!restoreParameters()) {
            outputs.error_message = "Parameters were not successfully restored";
        } else {
            outputs.status_message = "Parameters successfully restored";
        };
    }  

    // Reset Status Register when init function runs
    STATUS = readReg16Bit(STATUS_REG);

    // Get new status
    uint16_t RESET_STATUS = STATUS&0xFFFD;
    outputs.running = writeVerifyReg16Bit(STATUS_REG,RESET_STATUS); //reset POR Status  
    if (!outputs.running) {
        outputs.error_message = "Could not reset status flag, disabling reading fuel gauge";
    } else {
        outputs.status_message = "Fuel Gauge configured, with new config";
    } 
} else {
    outputs.status_message = "Fuel Gauge configured, Loading old config";
    outputs.running = true;
}
// @/
```
## Main subroutine
The main subroutine reads values from the registers and stores them in persistent outputs. This allows us to reset the fuel gauge with old values, after a hardware reset, TODO: Restoring old previous parameters has not been implemented.

The fuelgauge is only once every `inputs.pollrate` ms.

### Reading Battery Info

Battery status, inserted and removed are all stored in the status register. In order to read them we have to read the 2nd, 11th and 15th bit respectively from the register. The Battery status is 0 when there is a battery present, therefore it has to be inverted to be read

Both the battery insertion and removal are set to 1 once the event occurs and must be reset when read to prepare for a new event.

```cpp
//@='battinfo'
// Read battery status
uint16_t raw_status = readReg16Bit(STATUS_REG);

// Get the 4th bit
bool bat_status = raw_status&0x0800;
outputs.status = !bat_status; // battery status 0 when present, must invert
if (!outputs.status) {
  outputs.status_message = "No Battery Present";
}
// @/
```

### Reading Anolog Measurements and Model Outputs

The analog measurements are read from the 16bit registers. The Least significant bit is updated at the beginning of the routine in case it was updated by the user since the last run.

Both the raw and reported values are stored as persistent outputs. This helps with debugging and restoring old values in the case of hardware/software resets (ie: battery dies).

```cpp
//@='main'
        // Check restart
        agent.pollComponent(&this);

        static auto prev = sygsp::micros();
        auto now = sygsp::micros();
        // Check if properties have been updated
        if (inputs.designcap.updated) {
            outputs.running = writeDesignCapacity(); //Write Design Cap
        }
        if (inputs.ichg.updated) {
            outputs.running = writeICHG(); // End of charge current
        }
        if (inputs.vempty.updated || inputs.recovery_voltage.updated) {
            outputs.running = writeVoltage();
        }

        // Poll at fixed interval
        if (now-prev > (inputs.pollrate*1e3)) {
            prev = now;
            // BATTERY INFO
            @{battinfo}

            // Update outputs if there is no battery or the init failed don't read
            outputs.running = outputs.running & outputs.status;
            if (!outputs.running) return; // TODO: 

            // Compute capacity and current multipliers
            float curr_multiplier = base_current_multiplier_mAh / inputs.rsense;
            float cap_multiplier = base_capacity_multiplier_mAh /  inputs.rsense;
            
            // ANALOG MEASUREMENTS
            // Current
            int16_t inst_curr_raw = readReg16Bit(CURRENT_REG);
            int16_t avg_curr_raw = readReg16Bit(AVGCURRENT_REG);
            outputs.inst_curr = curr_multiplier * inst_curr_raw;
            outputs.avg_curr = curr_multiplier * avg_curr_raw;
            // Voltage
            uint16_t inst_voltage_raw = readReg16Bit(VCELL_REG);
            uint16_t avg_voltage_raw = readReg16Bit(AVGVCELL_REG);
            outputs.inst_voltage = voltage_multiplier_V * inst_voltage_raw;
            outputs.avg_voltage = voltage_multiplier_V * avg_voltage_raw;
            // MODEL OUTPUTS
            // Capacity
            uint16_t capacity_raw = readReg16Bit(REPCAP_REG);
            outputs.fullcapacity_raw = readReg16Bit(FULLCAP_REG);
            outputs.fullcapacitynom_raw = readReg16Bit(FULLCAPNORM_REG);
            outputs.capacity = cap_multiplier * capacity_raw;
            outputs.fullcapacity = cap_multiplier * outputs.fullcapacity_raw;

            // SOC, Age
            uint16_t age_raw = readReg16Bit(AGE_REG);
            uint16_t soc_raw = readReg16Bit(REPSOC_REG);
            outputs.age = percentage_multiplier * age_raw;
            outputs.soc = percentage_multiplier * soc_raw;
            // TTF,TTE
            uint16_t tte_raw = readReg16Bit(TTE_REG);
            uint16_t ttf_raw = readReg16Bit(TTF_REG);
            outputs.tte = time_multiplier_Hours * tte_raw;
            outputs.ttf = time_multiplier_Hours * ttf_raw;
            // Cycles
            outputs.chargecycles_raw = readReg16Bit(CYCLES_REG);
            outputs.chargecycles = 0.01f * outputs.chargecycles_raw;
            // Parameters
            outputs.rcomp =  readReg16Bit(RCOMPP0_REG);
            outputs.tempco = readReg16Bit(TEMPCO_REG);
        }
// @/
```


TODO: Tests

```cmake
# @#'CMakeLists.txt'
set(lib sygsa-max17055)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygah-metadata sygsp-delay sygsp-micros sygsp-restart-agent sygah-endpoints)
target_link_libraries(${lib} INTERFACE sygsp-arduino_hack)
# @/
```
