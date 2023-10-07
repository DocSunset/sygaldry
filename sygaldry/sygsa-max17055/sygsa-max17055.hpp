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
        int rsense; // Resistance of current sense resistor (mOhm))
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
        range_<-32768, 32767, 0> inst_curr_raw;
        range_<-32768, 32767, 0> avg_curr_raw;
        range_<num_literal<-5.12f>, 5.12f, 0.0f> inst_curr;
        range_<-5.12f, 5.12f, 0.0f> avg_curr;
        // Voltage
        range_<0, 65535, 0> inst_voltage_raw;
        range_<0, 65535, 0> avg_voltage_raw;
        range_<0.0f, 5.11992f, 0.0f> inst_voltage;
        range_<0.0f, 5.11992f, 0.0f> avg_voltage;

        // MODEL OUTPUTS
        // Capacity
        range_<0, 65535, 0> capacity_raw; // LSB
        range_<0, 65535, 0> fullcapacity_raw; // LSB
        range_< 0, 32000, 0> capacity; //mAh
        range_< 0, 32000, 0> fullcapacity; // mAh
        // Capacity (norm)
        range_<0,65535,0> fullcapacitynorm_raw;
        range_<0.0f,255.9961f,0.0f> fullcapacitynorm; // percentage
        // SOC, Age
        range_<0,65535,0> soc_raw; // LSB
        range_<0,65535,0> age_raw; // LSB
        range_<0.0f,255.9961f,0.0f> soc; // percentage
        range_<0.0f,255.9961f,0.0f> age; // percentage
        // Time to full (TTF), Time to empty (TTE), age
        range_<0,65535,0> ttf_raw; // LSB
        range_<0,65535,0> tte_raw; // LSB
        range_<0.0f,102.3984f,0.0f> ttf; // hours
        range_<0.0f,102.3984f,0.0f> tte;  // hours
        // Cycles
        range_<0,65535,0> chargecyles_raw;
        range_<0.0f,655.35f,0.0f> chargecyles;
        // Parameters
        persistent<uint16_t> rcomp;
        persistent<uint16_t> tempco;
        persistent<uint16_t> soc_stored;
        persistent<uint16_t> fullcap;
        persistent<uint16_t> fullcapnorm;
        persistent<uint16_t> cyles_stored;

        // Battery Status
        bng<"present"> status;
        bng<"removed"> removed;
        bng<"inserted"> inserted;

        text_message<"error message"> error_message;

        toggle<"running"> running;
    } outputs;

    /// initialize the MAX17055 for continuous reading
    void init()
    {
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
            // Read the raw values from regisgters
            outputs.inst_curr_raw = readReg16Bit(inputs.i2c_addr, CURRENT_REG);
            outputs.avg_curr_raw = readReg16Bit(inputs.i2c_addr, AVGCURRENT_REG);

        }
    }
};

} }
