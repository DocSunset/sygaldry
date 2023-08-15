#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <numbers>
#include "ICM_20948.h"
#include "sygaldry-helpers-metadata.hpp"
#include "sygaldry-helpers-endpoints.hpp"
#include "sygaldry-helpers-mimu.hpp"

#include <stdio.h>

namespace sygaldry { namespace components {

struct ICM20948
: name_<"ICM20948 MIMU">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
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

    ICM_20948_I2C * icm = nullptr;

    /// Initialize the ICM20948. Ensure the `Wire.begin()` API is called before this subroutine runs.
    void init()
    {
        icm = new ICM_20948_I2C{};
        auto status = icm->begin(); // calls startupDefault and startupMagnetometer
        if (status != ICM_20948_Stat_Ok)
        {
            outputs.error_message = icm->statusString(status);
            outputs.running = false;
            return;
        }
        printf("icm setup successful");
        outputs.running = icm->isConnected();
    }

    void main()
    {
    }
};

} }
