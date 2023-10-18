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
static constexpr float base_capacity_multiplier_mAh = 5.0f; // base capacity multiplier divide by rsense(mOhms) to get LSB
static constexpr float base_current_multiplier_mAh = 1.5625f; // base current multiplier divide by rsense(mOhms) to get LSB
static constexpr float voltage_multiplier_V = 7.8125e-5; //refer to row "Voltage"
static constexpr float time_multiplier_Hours = 5.625f/3600.0f; //Least Significant Bit= 5.625 seconds, 3600 converts it to Hours.
static constexpr float percentage_multiplier = 1.0f/256.0f; //refer to row "Percentage"

