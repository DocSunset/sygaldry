/*
Copyright 2023 Albert-Ngabo Niyonsenga Input Devices and Music
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
, version_<"1.0.0">
{
    struct inputs_t {
        // Initialisation Elements
        uint8_t i2c_addr; // i2c address of the fuel guage
        slider<"capacity", "mAh", int, 0, 32000, 3200> designcap; // Design capacity of the battery (mAh)
        slider<"end-of-charge current", "mA", int, 0, 32000, 50> ichg; // End of charge current (mA)
        slider<"current sense resistor", "mOhm", int, 0, 100, 10> rsense; // Resistance of current sense resistor (mOhm))
        slider<"Empty Voltage", "V", float, 0.0f, 4.2f, 3.0f>  vempty; // Empty voltage of the battery (V)
        slider<"Recovery voltage", "V", float, 0.0f, 4.2f, 3.8f> recovery_voltage; // Recovery voltage of the battery (V)

        // Other parameters
        slider<"poll rate", "ms", int, 0, 300000, 0> pollrate; // poll rate in milliseconds
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
