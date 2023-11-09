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
#include "sygah-mimu.hpp"
#include "sygah-metadata.hpp"
#include "sygsp-icm20948_registers.hpp"
#include "sygsp-icm20948_tests.hpp"
#include "sygsp-delay.hpp"
#include "sygsp-micros.hpp"
#include "sygsp-mimu_units.hpp"

namespace sygaldry { namespace sygsp {
/// \addtogroup sygsp
/// \{
/// \defgroup sygsp-icm20948 sygsp-icm20948: ICM20948 MIMU Driver
/// \{

template<typename Serif, typename AK09916Serif>
struct ICM20948
: name_<"ICM20948 MIMU">
{
    struct inputs_t {
        // TODO: sensitivity, digital low pass filter controls, measurement rate, etc.
    } inputs;

    struct outputs_t {
        vec3_message<"accelerometer raw", int, -32768, 32767, "LSB"> accl_raw;
        slider<"accelerometer sensitivity", "g/LSB", float, 1/16384.0f, 1/2048.0f, 1/4096.0f> accl_sensitivity;
        vec3_message<"accelerometer", float, -16, 16, "g"> accl;

        vec3_message<"gyroscope raw", int, -32768, 32767, "LSB"> gyro_raw;
        slider<"gyroscope sensitivity", "(rad/s)/LSB", float, 1/131.0f * rad_per_deg, 1/16.4f * rad_per_deg, 1/16.4f * rad_per_deg> gyro_sensitivity;
        vec3_message<"gyroscope", float, -2000.0f * rad_per_deg, 2000.0f * rad_per_deg, "rad/s"> gyro;

        vec3_message<"magnetometer raw", int, -32768, 32767, "LSB"> magn_raw;
        slider<"magnetometer sensitivity", "uT/LSB", float, 0.15f, 0.15f> magn_sensitivity;
        vec3_message<"magnetometer", float, -4900, 4900, "uT"> magn;

        slider_message<"elapsed", "time in microseconds elapsed since last measurement", unsigned long, 0, 1000000, 0> elapsed;

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

/// \}
/// \}
} }
