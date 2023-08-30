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

namespace sygaldry { namespace sygsp {

template<typename Serif>
struct ICM20948
: name_<"ICM20948 MIMU">
{
    // TODO: these constants probably don't belong here
    static constexpr float mss_per_g = 9.80665; // standard gravity according to Wikipedia, citing the International Bureau of Weights and Measures
    static constexpr float g_per_mss = 1.0/mss_per_g;
    static constexpr float rad_per_deg = std::numbers::pi / 180.0;
    static constexpr float deg_per_rad = 180.0 / std::numbers::pi;

    struct inputs_t {
        // TODO: sensitivity, digital low pass filter controls, measurement rate, etc.
    } inputs;

    struct outputs_t {
        vec3_message<"accelerometer raw", int, -32768, 32767, "LSB"> accl_raw;
        slider<"accelerometer sensitivity", "LSB/ms^2", float, 2048.0f * g_per_mss, 16384.0f * g_per_mss, 16384.0f * g_per_mss> accl_sensitivity;
        vec3_message<"accelerometer", float, -16 * mss_per_g, 16 * mss_per_g, "ms^2"> accl;

        vec3_message<"gyroscope raw", int, -32768, 32767, "LSB"> gyro_raw;
        slider<"gyroscope sensitivity", "LSB/(rad/s)", float, 16.4f * deg_per_rad, 131.0f * deg_per_rad, 131.0f * deg_per_rad> gyro_sensitivity;
        vec3_message<"gyroscope", float, -2000.0f * rad_per_deg, 2000.0f * rad_per_deg, "rad/s"> gyro;

        vec3_message<"magnetometer raw", int, -32768, 32767, "LSB"> magn_raw;
        slider<"magnetometer sensitivity", "LSB/uT", float, 0.15f, 0.15f, 0.15f> magn_sensitivity;
        vec3_message<"magnetometer", float, -4900, 4900, "uT"> magn;

        text_message<"error message"> error_message;

        toggle<"running"> running;
    } outputs;

    using Registers = ICM20948Registers<Serif>;

    void init()
    {
        outputs.running = true;
        if (!ICM20948Tests<Serif>::test()) outputs.running = false;
        if (!outputs.running) return;
        Registers::PWR_MGMT_1::DEVICE_RESET::trigger();
        delay(10);
        Registers::PWR_MGMT_1::SLEEP::disable();
        delay(10);
    }

    void main()
    {
        if (!outputs.running) return; // TODO: retry connecting every so often
        if (!Registers::INT_STATUS_1::read()) return;
        static constexpr uint8_t N_OUT = 1 + Registers::GYRO_ZOUT_L::address
                                           - Registers::ACCEL_XOUT_H::address;
        static_assert(N_OUT == 12);
        static uint8_t raw[N_OUT];
        uint8_t n_read = Serif::read(Registers::ACCEL_XOUT_H::address, raw, N_OUT);
        if (n_read != N_OUT) printf("only read %d out of %d\n", n_read, N_OUT);
        outputs.accl_raw = { raw[0] << 8 | (raw[1] & 0xFF)
                           , raw[2] << 8 | (raw[3] & 0xFF)
                           , raw[4] << 8 | (raw[5] & 0xFF)
                           };
        printf("%x %x %x\n", outputs.accl_raw.x(), outputs.accl_raw.y(), outputs.accl_raw.z());
    }
};

} }
