/*
Copyright 2023 Albert-Ngabo Niyonsenga Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada

SPDX-License-Identifier: MIT
*/
#pragma once
#include "sygsa-max17055.hpp"
#include "sygsp-delay.hpp"

namespace sygaldry { namespace sygsa {
    /// initialize the MAX17055 for continuous reading
    void MAX17055::init()
    {
        inputs.i2c_addr = inputs.i2c_addr.init();
        uint16_t STATUS = readReg16Bit(STATUS_REG);
        uint16_t POR = STATUS&0x0002;
        std::cout << "    Checking status " << "\n"
                << "    Status read: " << STATUS << "\n"
                << "    POR flag: " << POR << std::endl;
        // Reset the Fuel Gauge
        if (POR)
        {
            std::cout << "    Initialising Fuel Gauge" << std::endl;
            while(readReg16Bit(0x3D)&1) {
                sygsp::delay(10);
            }

            std::cout << "    Start up complete" << std::endl;
            //Initialise Configuration
            uint16_t HibCFG = readReg16Bit(0xBA);
            // Exit hibernate mode
            writeReg16Bit(0x60, 0x90);
            writeReg16Bit(0xBA, 0x0);
            writeReg16Bit(0x60, 0x0);
           //EZ Config
            // Write Battery capacity
            std::cout << "    Writing Capacity" << std::endl;
            uint16_t reg_cap = (inputs.designcap * inputs.rsense) / base_capacity_multiplier_mAh;
            uint16_t reg_ichg = (inputs.ichg * inputs.rsense) / base_current_multiplier_mAh;
            writeReg16Bit(DESIGNCAP_REG, reg_cap); //Write Design Cap
            writeReg16Bit(ICHTERM_REG, reg_ichg); // End of charge current
            writeReg16Bit(dQACC_REG, reg_cap/32); //Write dQAcc
            writeReg16Bit(dPACC_REG, 44138/32); //Write dPAcc

            // Set empty voltage and recovery voltage
            // Empty voltage in increments of 10mV
            std::cout << "    Writing Voltage" << std::endl;
            uint16_t reg_vempty = inputs.vempty * 100; //empty voltage in 10mV
            uint16_t reg_recover = 3.88 *25; //recovery voltage in 40mV increments
            uint16_t voltage_settings = (reg_vempty << 7) | reg_recover; 
            writeReg16Bit(VEMPTY_REG, voltage_settings); //Write Vempty 
            // Set Model Characteristic
            writeReg16Bit(MODELCFG_REG, 0x8000); //Write ModelCFG

            //Wait until model refresh
            while(readReg16Bit(MODELCFG_REG)&0x8000) {
                sygsp::delay(10);
            }
            //Reload original HbCFG value
            writeReg16Bit(0xBA,HibCFG);    
        } else {
            std::cout << "    Loading old config" << std::endl;
        }
          // Reset Status Register when init function runs
          std::cout << "    Resetting Status" << std::endl;
          STATUS = readReg16Bit(STATUS_REG);

          // Get new status
          uint16_t RESET_STATUS = STATUS&0xFFFD;
          std::cout << "    Setting new status: " << RESET_STATUS << std::endl;
          outputs.running = writeVerifyReg16Bit(STATUS_REG,RESET_STATUS); //reset POR Status  
          if (!outputs.running) {
            outputs.error_message = "Could not reset status flag, disabling reading fuel gauge";
          }
    }

    // poll the MAX17055 for new data and update endpoints
    void MAX17055::main()
    {
                static auto prev = sygsp::micros();
                auto now = sygsp::micros();
                if (now-prev > (inputs.pollrate*1e3)) {
                    prev = now;
                    // BATTERY INFO
                    // Read battery status
                    uint16_t raw_status = readReg16Bit(STATUS_REG);

                    // Get the 4th bit
                    bool bat_status = raw_status&0x0800;
                    outputs.status = !bat_status; // battery status 0 when present, must invert
                    if (!outputs.status) {
                      outputs.error_message = "No Battery Present";
                    }
                    // Get insertion
                    outputs.inserted = raw_status&0x0800; // Get the 11th bit
                    // Get removed
                    outputs.removed = raw_status&0x8000;  // get the 15th bit

                    // Reset Insertion bit
                    writeVerifyReg16Bit(STATUS_REG, raw_status&0xF7F);
                    // Reset Removal bit
                    writeVerifyReg16Bit(STATUS_REG, raw_status&0x7FFF);

                    // Update outputs if there is no battery or the init failed don't read
                    outputs.running = outputs.running & outputs.status;
                    if (!outputs.running) return; // TODO: 

                    // Compute capacity and current multipliers
                    float curr_multiplier = base_current_multiplier_mAh / inputs.rsense;
                    float cap_multiplier = base_capacity_multiplier_mAh /  inputs.rsense;
                    
                    // ANALOG MEASUREMENTS
                    // Current
                    outputs.inst_curr_raw = readReg16Bit(CURRENT_REG);
                    outputs.avg_curr_raw = readReg16Bit(AVGCURRENT_REG);
                    outputs.inst_curr = curr_multiplier * outputs.inst_curr;
                    outputs.avg_curr = curr_multiplier * outputs.avg_curr;
                    // Voltage
                    outputs.inst_voltage_raw = readReg16Bit(VCELL_REG);
                    outputs.avg_voltage_raw = readReg16Bit(AVGVCELL_REG);
                    outputs.inst_voltage = voltage_multiplier_V * outputs.inst_voltage_raw;
                    outputs.avg_voltage = voltage_multiplier_V * outputs.avg_voltage_raw;
                    // MODEL OUTPUTS
                    // Capacity
                    outputs.capacity_raw = readReg16Bit(REPCAP_REG);
                    outputs.fullcapacity_raw = readReg16Bit(FULLCAP_REG);
                    outputs.fullcapacitynorm_raw = readReg16Bit(FULLCAPNORM_REG);
                    outputs.capacity = cap_multiplier * outputs.capacity_raw;
                    outputs.fullcapacity = cap_multiplier * outputs.fullcapacity_raw;
                    outputs.fullcapacitynorm = percentage_multiplier * outputs.fullcapacitynorm_raw;
                    // SOC, Age
                    outputs.age_raw = readReg16Bit(AGE_REG);
                    outputs.soc_raw = readReg16Bit(REPSOC_REG);
                    outputs.age = percentage_multiplier * outputs.age_raw;
                    outputs.soc = percentage_multiplier * outputs.soc_raw;
                    // TTF,TTE
                    outputs.tte_raw = readReg16Bit(TTE_REG);
                    outputs.ttf_raw = readReg16Bit(TTF_REG);
                    outputs.tte = time_multiplier_Hours * outputs.tte_raw;
                    outputs.ttf = time_multiplier_Hours * outputs.ttf_raw;
                    // Cycles
                    outputs.chargecyles_raw = readReg16Bit(CYCLES_REG);
                    outputs.chargecyles = 0.01f * outputs.chargecyles_raw;
                    // Parameters
                    outputs.rcomp =  readReg16Bit(RCOMPP0_REG);
                    outputs.tempco = readReg16Bit(TEMPCO_REG);
                }
    }

    /// Read 16 bit register
    uint16_t MAX17055::readReg16Bit(uint8_t reg)
    {
        uint16_t value = 0;  
        Wire.beginTransmission(inputs.i2c_addr); 
        Wire.write(reg);
        Wire.endTransmission(false);
        
        Wire.requestFrom(inputs.i2c_addr, (uint8_t) 2); 
        value  = Wire.read();
        value |= (uint16_t)Wire.read() << 8;      // value low byte
        return value;
    }

    /// Write to 16 bit register
    void MAX17055::writeReg16Bit(uint8_t reg, uint16_t value)
    {
        //Write order is LSB first, and then MSB. Refer to AN635 pg 35 figure 1.12.2.5
        Wire.beginTransmission(inputs.i2c_addr);
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
            std::cout << "    Resetting Status ... attempt " << attempt << std::endl;
            //Write the value to the register
            writeReg16Bit(reg, value);
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
} 
