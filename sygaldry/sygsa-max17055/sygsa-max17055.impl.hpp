/*
Copyright 2023 Albert-Ngabo Niyonsenga Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada

SPDX-License-Identifier: MIT
*/
#pragma once
#include "sygsp-delay.hpp"
#include "sygsp-micros.hpp"
#include "sygsa-max17055-helpers.hpp"
#include <iostream>
#include "Wire.h"
#include "sygsa-max17055.hpp"
#include "sygsp-delay.hpp"

namespace sygaldry { namespace sygsa {
    /// initialize the MAX17055 for continuous reading
    void MAX17055::init()
    {
        // Initialise all the slider variables
        inputs.designcap = inputs.designcap.init();
        inputs.ichg = inputs.ichg.init();
        inputs.rsense = inputs.rsense.init();
        inputs.vempty = inputs.vempty.init();
        inputs.recovery_voltage = inputs.recovery_voltage.init();
        inputs.pollrate = inputs.pollrate.init();

        // Read the status registry and check for hardware/software reset
        uint16_t STATUS = readReg16Bit(STATUS_REG);
        uint16_t POR = STATUS&0x0002;
        std::cout << "Checking status " << "\n"
                << "Status read: " << STATUS << "\n"
                << "POR flag: " << POR << std::endl;
        // Reset the Fuel Gauge
        if (POR)
        {
            std::cout << "Initialising Fuel Gauge" << std::endl;
            while(readReg16Bit(0x3D)&1) {
                sygsp::delay(10);
            }

            std::cout << "Start up complete" << std::endl;
            //Initialise Configuration
            uint16_t HibCFG = readReg16Bit(0xBA);
            // Exit hibernate mode
            writeReg16Bit(0x60, 0x90);
            writeReg16Bit(0xBA, 0x0);
            writeReg16Bit(0x60, 0x0);
           //EZ Config
            // Write Battery capacity
            std::cout << "Writing Capacity" << std::endl;
            writeDesignCapacity(); //Write Design Cap
            writeICHG(); // End of charge current
            writeReg16Bit(dPACC_REG, 44138/32); //Write dPAcc

            // Set empty voltage and recovery voltage
            // Empty voltage in increments of 10mV
            std::cout << "Writing Voltage" << std::endl;
            writeVoltage();
            // Set Model Characteristic
            writeReg16Bit(MODELCFG_REG, 0x8000); //Write ModelCFG

            //Wait until model refresh
            while(readReg16Bit(MODELCFG_REG)&0x8000) {
                sygsp::delay(10);
            }
            //Reload original HbCFG value
            writeReg16Bit(0xBA,HibCFG); 

            // Restore old parameters
            if (outputs.fullcapacitynom_raw != 0) {
                if (!restoreParameters()) {
                    outputs.error_message = "Parameters were not successfully restored";
                };
            }   
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
                // Check if properties have been updated
                if (inputs.designcap.updated) {
                    writeDesignCapacity(); //Write Design Cap
                }
                if (inputs.ichg.updated) {
                    writeICHG(); // End of charge current
                }
                if (inputs.vempty.updated || inputs.recovery_voltage.updated) {
                    writeVoltage();
                }

                // Poll at fixed interval
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
    }

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
            sygsp::delay(1);

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

    // helper functions for reading properties
    /// Write design capacity
    void MAX17055::writeDesignCapacity() {
        uint16_t reg_cap = (inputs.designcap * inputs.rsense) / base_capacity_multiplier_mAh;
        writeReg16Bit(DESIGNCAP_REG, reg_cap); //Write Design Cap
        writeReg16Bit(dQACC_REG, reg_cap/32); //Write dQAcc
    };

    /// Write end of charge current
    void MAX17055::writeICHG() {
        uint16_t reg_ichg = (inputs.ichg * inputs.rsense) / base_current_multiplier_mAh;
        writeReg16Bit(ICHTERM_REG, reg_ichg);
    };

    /// Write Vempty and recovery voltage
    void MAX17055::writeVoltage() {
        uint16_t reg_vempty = inputs.vempty * 100; //empty voltage in 10mV
        uint16_t reg_recover = 3.88 *25; //recovery voltage in 40mV increments
        uint16_t voltage_settings = (reg_vempty << 7) | reg_recover; 
        writeReg16Bit(VEMPTY_REG, voltage_settings); //Write Vempty 
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
        sygsp::delay(350);

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
        sygsp::delay(350);

        // Restore cycles
        if (!writeVerifyReg16Bit(CYCLES_REG, outputs.chargecycles_raw)) {
            return false;
        }

        // Return true when finished
        return true;
    };
}
} 