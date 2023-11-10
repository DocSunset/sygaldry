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
, version_<"1.0.0">
{
    struct inputs_t {
        // Initialisation Elements
        slider_message<"capacity", "mAh", int, 0, 32000, 2600> designcap; // Design capacity of the battery (mAh)
        slider_message<"end-of-charge current", "mA", int, 0, 32000, 50> ichg; // End of charge current (mA)
        slider_message<"current sense resistor", "mOhm", int, 0, 100, 10> rsense; // Resistance of current sense resistor (mOhm))
        slider_message<"Empty Voltage", "V", float, 0.0f, 4.2f, 3.0f>  vempty; // Empty voltage of the battery (V)
        slider_message<"Recovery voltage", "V", float, 0.0f, 4.2f, 3.8f> recovery_voltage; // Recovery voltage of the battery (V)

        // Other parameters
        slider_message<"poll rate", "ms", int, 0, 300000, 300000> pollrate; // poll rate in milliseconds
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
        slider<"full capacity percentage", "%", float, 0.0f, 255.9961f, 0.0f> fullcapacitynorm;
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
        slider<"raw charge cycles", "LSB", int, 0, 65535, 0> chargecycles_raw;
        slider<"charge cycles", "num", float, 0.0f, 655.35f, 0.0f> chargecycles;
        // Parameters
        slider<"rcomp", "LSB", int, 0, 65535, 0> rcomp;
        slider<"tempco", "LSB", int, 0, 65535, 0> tempco;

        // Battery Status
        toggle<"present"> status;

        text_message<"error message"> error_message;
        text_message<"status message"> status_message;

        toggle<"running"> running;
    } outputs;

    // initialize the MAX17055 for continuous reading
    void init();

    // poll the MAX17055 for new data and update endpoints
    void main();

    // Read 16 bit register
    uint16_t readReg16Bit(uint8_t reg);
    
    // Write to 16 bit register
    void writeReg16Bit(uint8_t reg, uint16_t valu);
    
    // Write and verify to 16 bit register
    bool writeVerifyReg16Bit(uint8_t reg, uint16_t value);

    // Write design capacity
    void writeDesignCapacity();

    // Write end of charge current
    void writeICHG();

    // Write Vempty and recovery voltage
    void writeVoltage();

    // Restore old parameters
    bool restoreParameters();
};

} }
