\page page-sygsp-mimu_fusion MIMU Sensor Fusion

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This component performs runtime calibration and sensor fusion of magnetic
and inertial measurements from a MIMU device such as
[the ICM20948](\ref page-sygsp-icm20948).

```cpp
// @#'sygsp-complementary_mimu_fusion_endpoints.hpp'
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
// @/
```

```cpp
// @#'sygsp-complementary_mimu_fusion.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <array>
#include "sygac-mimu.hpp"
#include "sygsp-complementary_mimu_fusion_endpoints.hpp"

namespace sygaldry { namespace sygsp {

/*! \addtogroup sygsp
*/
/// \{

/*! \defgroup sygsp-mimu_fusion MIMU Sensor Fusion
*/
/// \{

/*! Core complementary MIMU fusion filter initialization

\param[in,out] in Filter input endpoints to initialize
\param[in,out] out Filter output endpoints to initialize
*/
void complementary_mimu_fusion_init( ComplementaryMimuFusionInputs& in
                                   , ComplementaryMimuFusionOutputs& out
                                   );


/*! Core complementary MIMU fusion filter routine

\param[in] gyro Angular rate measurement from gyroscope. A gyroscope measurement must always be given.
\param[in] accl Acceleration measurement from accelerometer. Set to all 0 if no measurement is available.
\param[in] magn Magnetic field measurement from magnetometer. Set to all 0 if no measurement is available.
\param[in] elapsed Time elapsed since last update (i.e. measurement period).
\param[in] in Filter input endpoints
\param[out] out Filter output endpoints
*/
void complementary_mimu_fusion( const std::array<float, 3>& gyro
                              , const std::array<float, 3>& accl, bool accl_updated
                              , const std::array<float, 3>& magn, bool magn_updated
                              , const unsigned long elapsed
                              , const ComplementaryMimuFusionInputs& in
                              , ComplementaryMimuFusionOutputs& out
                              );

/*! \brief MIMU sensor fusion and runtime calibration component

Based on measurements of the magnetic field, acceleration, and
angular rate from a MIMU sensor, estimate the orientation of the
sensor with respect to the global frame of reference, and various
other signals of interest.
*/
template<MimuComponent Mimu>
struct ComplementaryMimuFusion
: name_<"Complementary MIMU Fusion Filter">
, description_<"Runtime calibration and sensor fusion using a complementary filter approach">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    ComplementaryMimuFusionInputs inputs;

    ComplementaryMimuFusionOutputs outputs;

    /// Initialize the filter
    void init()
    {
        complementary_mimu_fusion_init(inputs, outputs);
    }

    /// Update the filter
    void main(const Mimu& mimu)
    {
        if (gyro_of(mimu))
            complementary_mimu_fusion( gyro_of(mimu)
                                     , accl_of(mimu), accl_of(mimu)
                                     , magn_of(mimu), magn_of(mimu)
                                     , mimu.outputs.elapsed
                                     , inputs, outputs);
    }
};

/// \}
/// \}

} }
// @/
```

```cpp
// @#'sygsp-complementary_mimu_fusion.cpp'
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsp-complementary_mimu_fusion.hpp"

#include <Eigen/Eigen>
#include "sygsp-mimu_units.hpp"

namespace {

typedef Eigen::Vector3f Vector;
typedef Eigen::Matrix3f Matrix;
typedef Eigen::Quaternionf Quaternion;

void filter_state_init(sygaldry::sygsp::ComplementaryMimuFusionOutputs& out)
{
    out.quaternion = {1,0,0,0};
    out.conjugate = {1,0,0,0};
    out.matrix  = {1,0,0, 0,1,0, 0,0,1};
    out.inverse = {1,0,0, 0,1,0, 0,0,1};
    out.norm_of_gravity = 1;
    out.g_count = 1;
    out.sensor_accl = {0};
    out.global_accl = {0};
    out.accl_prev = {0};
    out.jerk = {0};
    out.gyro_bias = {0};
    out.magn_max = {0};
    out.magn_min = {0};
    out.magn_bias = {0};
    out.angular_rate = {0};
    out.integral_feedback = {0};
    out.stationary = false;
}

}

namespace sygaldry { namespace sygsp {

void complementary_mimu_fusion_init(ComplementaryMimuFusionInputs& in, ComplementaryMimuFusionOutputs& out)
{
    in.initialize();
    in.calibrate_magnetometer = in.calibrate_magnetometer.init();
    filter_state_init(out);
}

void complementary_mimu_fusion( const std::array<float, 3>& gyro
                              , const std::array<float, 3>& accl, bool accl_updated
                              , const std::array<float, 3>& magn, bool magn_updated
                              , const unsigned long elapsed
                              , const ComplementaryMimuFusionInputs& in
                              , ComplementaryMimuFusionOutputs& out
                              )
{
    Eigen::Map<const Vector> omega{gyro.data()};
    Eigen::Map<const Vector> v_a{accl.data()};
    Vector v_m{magn.data()};
    Vector w_mes = Vector::Zero();
    Eigen::Map<Matrix> inverse{out.inverse.state.data()};
    auto v_hat_a = inverse.col(2); // fusion estimated "up" (gravity) vector (normalized) expressed in sensor frame
    auto v_hat_m = inverse.col(0); // estimated cross product of gravity and magnetic field
    if (accl_updated)
    {
        // detect motion
        Eigen::Map<Vector> v_anm1{out.accl_prev.state.data()};
        Eigen::Map<Vector> v_dot_a{out.jerk.state.data()};
        Vector v_avg_a = 0.5 * v_a + 0.5 * v_anm1; // lightly low pass filter accelerometer measurement
        v_dot_a = v_avg_a - v_anm1; out.jerk.updated = true;
        out.stationary = v_dot_a.dot(v_dot_a) < in.movement_threshold;
        v_anm1 = v_a; out.accl_prev.updated = true;

        if (out.stationary && out.g_count < out.g_count.max())
        {
            out.norm_of_gravity += (v_a.norm() - out.norm_of_gravity) / out.g_count;
            out.g_count += 1;
        }

        // update estimated linear acceleration due to motion
        Eigen::Map<Vector> v_a_lin{out.sensor_accl.state.data()};
        Eigen::Map<Vector> v_a_lin_gf{out.global_accl.state.data()};
        v_a_lin = mss_per_g * (v_a - v_hat_a * out.norm_of_gravity); out.sensor_accl.updated = true;
        v_a_lin_gf = inverse * v_a_lin; out.global_accl.updated = true;

        // estimate rotation according to accelerometer as equal to the cross product of measured vs estimated up vector
        w_mes += v_a.normalized().cross(v_hat_a) * in.k_a;
    }
    if (magn_updated)
    {
        if (in.calibrate_magnetometer)
        {
            // try to estimate and compensate for magnetometer measurement error
            // this naive first attempt simply tracks the extrema of the magnetometer measurements
            // and uses this to estimate bias
            // this assumes that the error in the measurement doesn't change over time
            // (no scaling, axis non-orthogonality, or rotation issues are addressed by this)
            // This approach also probably isn't very good since the
            // only retained measurements are by definition probably outliers.
            // Despite its limitations, this method works better than anything else I've tried
            Eigen::Map<Vector> v_m_maxima{out.magn_max.state.data()};
            Eigen::Map<Vector> v_m_minima{out.magn_min.state.data()};
            Eigen::Map<Vector> v_m_bias{out.magn_bias.state.data()};
            Vector max = v_m_maxima.cwiseMax(v_m);
            Vector min = v_m_minima.cwiseMin(v_m);
            if ((v_m_maxima.array() != max.array()).any() || (v_m_minima.array() != min.array()).any())
            {
                v_m_maxima = max; out.magn_max.updated = true;
                v_m_minima = min; out.magn_min.updated = true;
                v_m_bias = 0.5 * (min + max); out.magn_bias.updated = true;
            }
            v_m = v_m - v_m_bias;
        }

        // estimate rotation according to magnetometer as cross product of measured vs estimated east vector
        v_m = v_hat_a.cross(v_m); // measured cross-product of gravity and magnetic field
        v_m.normalize(); // normalize after the cross product so that the magnetic dip angle doesn't influence its magnitude
        w_mes += v_m.cross(v_hat_m) * in.k_m;
    }

    Eigen::Map<Vector> omega_bias{out.gyro_bias.state.data()};
    if (out.stationary)
    {
        // update gyro bias estimate
        omega_bias = (1.0 - in.gyro_alpha) * omega + in.gyro_alpha * omega_bias;
        out.gyro_bias.updated = true;
    }

    // reject gyro bias
    Eigen::Map<Vector> omega_hat{out.angular_rate.state.data()};
    omega_hat = omega - omega_bias;

    if (w_mes.x() != 0 || w_mes.y() != 0 || w_mes.z() != 0)
    {
        Eigen::Map<Vector> b_hat{out.integral_feedback.state.data()};
        // error correction is added to omega (angular rate) before integrating
        if (in.k_I > 0.0)
        {
            b_hat += w_mes * (double)elapsed/1000000.0; // see eq. (48c)
            omega_hat += in.k_P * w_mes + in.k_I * b_hat;
        }
        else
        {
            b_hat.setZero(); // Madgwick: "prevent integral windup"
            omega_hat += in.k_P * w_mes;
        }
        out.integral_feedback.updated = true;
    }

    out.angular_rate.updated = true;

    Eigen::Map<Quaternion> q{out.quaternion.state.data()};
    auto q_dot = q * Quaternion(0, omega_hat.x(), omega_hat.y(), omega_hat.z());
    q.coeffs() += 0.5 * q_dot.coeffs() * (double)elapsed/1000000.0;
    q.normalize(); out.quaternion.updated = true;
    Eigen::Map<Quaternion> conjugate{out.conjugate.state.data()};
    Eigen::Map<Matrix> matrix{out.matrix.state.data()};
    conjugate = q.conjugate(); out.conjugate.updated = true;
    matrix = q.toRotationMatrix(); out.matrix.updated = true;
    inverse = conjugate.toRotationMatrix(); out.inverse.updated = true;
}

} }
// @/
```

```cpp
// #'sygsp-complementary_mimu_fusion.test.cpp'
#include <catch2/catch_test_macros.hpp>
#include "sygsp-mimu_fusion.hpp"

using namespace sygaldry;
using namespace sygaldry::sygsp;

// TODO implement this and renable the lili chunks
{tests}
// /
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsp-complementary_mimu_fusion)
add_library(${lib} STATIC)
    target_sources(${lib}
            PRIVATE ${lib}.cpp
            )
    target_include_directories(${lib}
            PUBLIC .
            )
    target_link_libraries(${lib}
            PUBLIC sygah-endpoints
            PUBLIC sygah-metadata
            PUBLIC sygah-mimu
            PUBLIC sygac-mimu
            PUBLIC sygsp-mimu_units
            PRIVATE Eigen3::Eigen
            )

# TODO: write tests
# @/
```

