#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-endpoints.hpp"
#include "sygah-mimu.hpp"
#include "sygsp-mimu_units.hpp"

namespace sygaldry { namespace sygsp {

/*! \addtogroup sygsp
*/
/// \{

/*! \addtogroup sygsp-mimu_fusion MIMU Sensor Fusion
*/
/// \{

/// Inputs for the MIMU sensor fusion component
struct ComplementaryMimuFusionInputs {
    /* TODO:
        - adjusting sensor alignment with device
        - rotation of global frame into performance frame
        - filter coefficient adjustments
    */
    slider<"proportional feedback"
          , "proportional feedback gain coefficient"
          , float, 0.0f, 10.0f, 3.0f
          , tag_session_data
          > k_P;
    slider<"integral feedback"
          , "integral feedback gain coefficient"
          , float, 0.0f, 10.0f, 0.0f
          , tag_session_data
          > k_I;
    slider<"accelerometer influence"
          , "accelerometer gain coefficient; reflects confidence in the accelerometer data"
          , float, 0.0f, 1.0f, 0.25f
          , tag_session_data
          > k_a;
    slider<"magnetometer influence"
          , "magnetometer gain coefficient; reflects confidence in the magnetometer data"
          , float, 0.0f, 1.0f, 1.0f
          , tag_session_data
          > k_m;
    slider<"movement threshold"
          , "threshold of jerk magnitude below which the device is considered as stationary"
          , float, 0.0f, 32.0f, 0.0001f
          , tag_session_data
          > movement_threshold;
    slider<"gyroscope bias smoothing"
          , "IIR filter coefficient for gyro bias smoothing filter; higher value results in more smoothing/lower cutoff"
          , float, 0.0f, 1.0f, 0.99f
          , tag_session_data
          > gyro_alpha;
    bng<"initialize", "trigger sensor re-initialization"> initialize;
    toggle<"calibrate magnetometer"
          , "enable tracking magnetometer data to estimate and remove sensor bias; "
            "the previous bias estimate is reset when compensation is enabled"
          > calibrate_magnetometer;
    //bng<"zero"
    //    , "set performance-frame alignment by rotating the performance frame "
    //      "so that its y-axis aligns with the current sensor-frame y-axis"
    //    > zero;
};

/// Outputs of the MIMU sensor fusion component
struct ComplementaryMimuFusionOutputs {
    array_message<"sensor to global quaternion", 4
                 , "quaternion representation of the orientation of the sensor in the global frame"
                 , float, -1.0f, 1.0f
                 > quaternion;
    array_message<"global to sensor quaternion", 4
                 , "quaternion representation of the orientation of the global bases in the sensor frame"
                 , float, -1.0f, 1.0f
                 > conjugate;
    array_message<"sensor to global matrix", 9
                 , "special orthogonal matrix of the sensor basis vectors expressed in the global frame"
                 , float, -1.0f, 1.0f
                 > matrix;
    array_message<"global to sensor matrix", 9
                 , "special orthogonal matrix of the global basis vectors expressed in the sensor frame"
                 , float, -1.0f, 1.0f
                 > inverse;
    slider<"norm of gravity", "estimated strength of gravity relative to one standard gravity (9.80665 m/s/s)"
          , float, 0.995f, 1.002f
          > norm_of_gravity;
    slider<"gravity measurements", "count of gravity measurements"
          , unsigned int, 0, 1<<16
          > g_count;
    vec3_message<"sensor acceleration"
                , float, -16.0f * mss_per_g, 16.0f * mss_per_g
                , "m/s/s", "estimated acceleration due to motion expressed in the sensor frame of reference"
                > sensor_accl;
    vec3_message<"global acceleration"
                , float, -16.0f * mss_per_g, 16.0f * mss_per_g
                , "m/s/s", "estimated acceleration due to motion expressed in the performance frame of reference"
                > global_accl;
    vec3_message<"previous accelerometer"
                , float, -16.0f, 16.0f
                , "g", "previous measurement of acceleration; used in the estimation of jerk"
                > accl_prev;
    vec3_message<"jerk"
                , float, -32.0f, 32.0f
                , "g/s", "approximate signal analogous to the derivative of acceleration in the sensor frame"
                > jerk;
    vec3_message<"gyroscope bias"
                , float, -2000.0f * rad_per_deg, 2000.0f * rad_per_deg
                , "rad/s", "estimated gyroscope sensor bias"
                > gyro_bias;
    vec3_message<"magnetometer maximum"
                , float, -4900.0f, 4900.0f
                , "uT", "component-wise maximum of magnetometer values measured while calibrating bias compensation"
                > magn_max;
    vec3_message<"magnetometer minimum"
                , float, -4900.0f, 4900.0f
                , "uT", "component-wise minimum of magnetometer values measured while calibrating bias compensation"
                > magn_min;
    vec3_message<"magnetometer bias"
                , float, -4900.0f, 4900.0f
                , "uT", "estimated magnetometer sensor bias"
                > magn_bias;
    vec3_message<"angular rate"
                , float, -2000.0f * rad_per_deg, 2000.0f * rad_per_deg
                , "rad/s", "estimated angular rate based on fused sensor measurements"
                > angular_rate;
    vec3_message< "integral feedback"
                , float, -20.0f, 20.0f
                , "integral feedback through the complementary filter"
                > integral_feedback;
    toggle<"stationary"
          , "boolean indication of whether the sensor is stationary (1) or movement is detected (0)"
          > stationary;
};

/// \}
/// \}

} }
