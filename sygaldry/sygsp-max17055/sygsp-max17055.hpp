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
#include "sygsp-max17055_tests.hpp"
#include "sygsp-delay.hpp"
#include "sygsp-micros.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace sygsp {

template<typename Serif, typename AK09916Serif>
struct ICM20948
: name_<"MAX17055 Fuel Gauge">
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
    } inputs;

    struct outputs_t {

        
        // Battery info variables
        bool bat_status = false;            // Boolean for if a battery is present in the system
        bool bat_insert = false;            // Boolean for if a battery is inserted in the system
        bool bat_remove = false;            // Boolean for if a battery is removed from the system

        // learned variables
        uint16_t rcomp = 0;                 // Characterisation information for computing open-circuit voltage of cell
        uint16_t tempco = 0;                // Temperature compensation informtion for rcomp0
        uint16_t fullcap = 0;               // Capacity of the battery when full (mAh)
        uint16_t cycles = 0;                // Number of charge cycles

        // ANALOG MEASUREMENTS
        // Current
        range_<-32768, 32767, 0> inst_curr_raw;
        range_<-32768, 32767, 0> avg_curr_raw;
        range_<1/64000.0f, 10/64f, 1/640.0f> curr_sensitivity;
        range_<-5.12f, 5.12f, 0> inst_curr;
        range_<-5.12f, 5.12f, 0> avg_curr;
        // Voltage
        range_<0, 65535, 0> inst_voltage_raw;
        range_<0, 65535, 0> avg_voltage_raw;
        range_<0.0f, 5.11992f, 0.0f> inst_voltage;
        range_<0.0f, 5.11992f, 0.0f> avg_voltage;

        // MODEL OUTPUTS
        // Capacity
        range_<0, 65535, 0> capacity_raw; // LSB
        range_<0, 65535, 0> fullcapacity_raw; // LSB
        range_<0.005f, 5f, 0.5f> capacity_sensitivity; // mAh/LSB
        range_< 0.0f, 32000.0f, 0> capacity; //mAh
        range_< 0.0f, 32000.0f, 0> fullcapacity; // mAh
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
        range_<0,65535,0> rcomp_out;
        range_<0,65535,0> tempcomp_out;

        text_message<"error message"> error_message;

        toggle<"running"> running;
    } outputs;

    using Registers = ICM20948Registers<Serif>;
    using AK09916Registers = ICM20948Registers<AK09916Serif>;

    /// initialize the ICM20948 for continuous reading
    void init()
    {
        outputs.running = true;
        if (!ICM20948Tests<Serif, AK09916Serif>::test()) outputs.running = false;
        if (!outputs.running) return;
        Registers::PWR_MGMT_1::DEVICE_RESET::trigger(); delay(10); // reset (establish known preconditions)
        Registers::PWR_MGMT_1::SLEEP::disable(); delay(10); // disable sleep
        Registers::INT_PIN_CFG::BYPASS_EN::enable(); delay(1); // bypass the I2C controller, connecting the aux bus to the main bus
        Registers::GYRO_CONFIG_1::GYRO_FS_SEL::DPS_2000::set();
        Registers::ACCEL_CONFIG::ACCEL_FS_SEL::G_8::set();
        AK09916Registers::CNTL3::SRST::trigger(); delay(1); // soft-reset the magnetometer (establish known preconditions)
        AK09916Registers::CNTL2::MODE::ContinuousMode100Hz::set(); delay(1); // enable continuous reads
        outputs.accl_sensitivity = outputs.accl_sensitivity.init();
        outputs.gyro_sensitivity = outputs.gyro_sensitivity.init();
        outputs.magn_sensitivity = outputs.magn_sensitivity.init();
    }

    // poll the ICM20948 for new data and update endpoints
    void main()
    {
        if (!outputs.running) return; // TODO: retry connecting every so often

        static constexpr uint8_t IMU_N_OUT = 1 + Registers::GYRO_ZOUT_L::address
                                               - Registers::ACCEL_XOUT_H::address;
        static constexpr uint8_t MAG_N_OUT = 1 + AK09916Registers::ST2::address
                                               - AK09916Registers::HXL::address;
        static_assert(IMU_N_OUT == 12);
        static_assert(MAG_N_OUT == 8);

        static uint8_t raw[IMU_N_OUT];
        static auto prev = micros();
        auto now = micros();
        bool read = false;
        if (Registers::INT_STATUS_1::read())
        {
            read = true;
            Serif::read(Registers::ACCEL_XOUT_H::address, raw, IMU_N_OUT);
            outputs.accl_raw = { (int)(int16_t)( raw[0] << 8 | ( raw[1] & 0xFF))
                               , (int)(int16_t)( raw[2] << 8 | ( raw[3] & 0xFF))
                               , (int)(int16_t)( raw[4] << 8 | ( raw[5] & 0xFF))
                               };
            outputs.gyro_raw = { (int)(int16_t)( raw[6] << 8 | ( raw[7] & 0xFF))
                               , (int)(int16_t)( raw[8] << 8 | ( raw[9] & 0xFF))
                               , (int)(int16_t)(raw[10] << 8 | (raw[11] & 0xFF))
                               };
            outputs.accl = { outputs.accl_raw.x() * outputs.accl_sensitivity
                           , outputs.accl_raw.y() * outputs.accl_sensitivity
                           , outputs.accl_raw.z() * outputs.accl_sensitivity
                           };
            outputs.gyro = { outputs.gyro_raw.x() * outputs.gyro_sensitivity
                           , outputs.gyro_raw.y() * outputs.gyro_sensitivity
                           , outputs.gyro_raw.z() * outputs.gyro_sensitivity
                           };
        }
        if (AK09916Registers::ST1::DRDY::read_field())
        {
            read = true;
            AK09916Serif::read(AK09916Registers::HXL::address, raw, MAG_N_OUT);
            outputs.magn_raw = { (int)(int16_t)( raw[1] << 8 | ( raw[0] & 0xFF))
                               , (int)(int16_t)( raw[3] << 8 | ( raw[2] & 0xFF))
                               , (int)(int16_t)( raw[5] << 8 | ( raw[4] & 0xFF))
                               };
            outputs.magn = { outputs.magn_raw.x() * outputs.magn_sensitivity
                           , -outputs.magn_raw.y() * outputs.magn_sensitivity
                           , -outputs.magn_raw.z() * outputs.magn_sensitivity
                           };
        }
        if (read)
        {
            outputs.elapsed = now - prev;
            prev = now;
        }
    }
};

} }
