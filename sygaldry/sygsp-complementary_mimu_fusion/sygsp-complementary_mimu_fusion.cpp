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
