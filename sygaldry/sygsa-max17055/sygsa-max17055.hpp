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

template<int capacity = default_capacity, int current_sense_resistor = default_rsense, int poll_rate = default_poll_rate, int end_of_charge_current = default_ichg, float empty_voltage = default_vempty, float recovery_voltage_in = default_recovery_voltage>
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
        slider_message<"restart policy","Set the restart policy for the component", 1, 4, 1, tag_session_data> restart_policy;
        slider_message<"restart attempts","Set the max amount of restart attempts", 0, 10, 0, tag_session_data> max_attempts;
        toggle<"stop signal", "Indicate that the fuel gauge should stop running", 0, tag_session_data> stop_signal;
        toggle<"attempt restart", "Indicates if the fuel gauge attempts to restart when it failed."> attempt_restart;
        slider_message<"restart time","Set the time between restart attempts", 5000, 30000, 5000, tag_session_data> restart_time;

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

        // Toggles
        toggle<"running", "Indicate if fuel gauge is running"> running;

        slider_message<"current attempt", "Current attempt for restarting fuel guage"> curr_attempt; // Current restart attempt
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
