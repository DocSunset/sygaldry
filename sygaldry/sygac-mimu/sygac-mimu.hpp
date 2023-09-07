#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <utility>
#include "sygac-components.hpp"

namespace sygaldry {

#define try_spelling(SPELLING)\
if constexpr (requires {mimu_data.SPELLING;}) return mimu_data.SPELLING; /* member acces */ \
else if constexpr (requires {mimu_data.SPELLING();}) return mimu_data.SPELLING(); /* member function */ \
else if constexpr (requires {mimu_data->SPELLING;}) return mimu_data->SPELLING; /* member through pointer */ \
else if constexpr (requires {mimu_data->SPELLING();}) return mimu_data->SPELLING() /* member function through pointer */

/// Access the accelerometer data of a presumed MIMU data structure
auto& accl_of(auto& mimu_data)
{
    try_spelling(accelerometer);
    else try_spelling(acceleration);
    else try_spelling(accel);
    else try_spelling(accl);
    else try_spelling(acc);
    else try_spelling(a);
    else return; // mimu_data is not MIMU data!
}

/// Access the gyroscope data of a presumed MIMU data structure
auto& gyro_of(auto& mimu_data)
{
    try_spelling(gyroscope);
    else try_spelling(angular_rate);
    else try_spelling(gyro);
    else try_spelling(g);
    else return; // mimu_data is not MIMU data!
}

/// Access the magnetometer data of a presumed MIMU data structure
auto& magn_of(auto& mimu_data)
{
    try_spelling(magnetometer);
    else try_spelling(magnetic_field);
    else try_spelling(magn);
    else try_spelling(mag);
    else try_spelling(m);
    else return; // mimu_data is not MIMU data!
}

/// Access the first vector component of a presumed MIMU data vector
auto& vecx_of(auto& mimu_vec)
{
    try_spelling(x);
    else if constexpr (requires {mimu_vec[0];}) return mimu_vec[0];
    else return; // mimu_vec is not MIMU data!
}

/// Access the second vector component of a presumed MIMU data vector
auto& vecy_of(auto& mimu_vec)
{
    try_spelling(y);
    else if constexpr (requires {mimu_vec[1];}) return mimu_vec[1];
    else return; // mimu_vec is not MIMU data!
}

/// Access the third vector component of a presumed MIMU data vector
auto& vecz_of(auto& mimu_vec)
{
    try_spelling(z);
    else if constexpr (requires {mimu_vec[2];}) return mimu_vec[2];
    else return; // mimu_vec is not MIMU data!
}

#undef try_spelling
/// Access the type of the accelerometer data of a presumed MIMU data structure
template<typename T> using accl_t
    = std::remove_cvref_t<decltype(accl_of(std::declval<std::remove_cvref_t<T>&>()))>;
/// Access the type of the gyroscope data of a presumed MIMU data structure
template<typename T> using gyro_t
    = std::remove_cvref_t<decltype(gyro_of(std::declval<std::remove_cvref_t<T>&>()))>;
/// Access the type of the magnetometer data of a presumed MIMU data structure
template<typename T> using magn_t
    = std::remove_cvref_t<decltype(magn_of(std::declval<std::remove_cvref_t<T>&>()))>;
/// Access the type of the x component of a presumed MIMU data vector
template<typename T> using vecx_t
    = std::remove_cvref_t<decltype(vecx_of(std::declval<std::remove_cvref_t<T>&>()))>;
/// Access the type of the y component of a presumed MIMU data vector
template<typename T> using vecy_t
    = std::remove_cvref_t<decltype(vecy_of(std::declval<std::remove_cvref_t<T>&>()))>;
/// Access the type of the z component of a presumed MIMU data vector
template<typename T> using vecz_t
    = std::remove_cvref_t<decltype(vecz_of(std::declval<std::remove_cvref_t<T>&>()))>;
/// Check that type `T` is not `void`. Used in the definition of `MimuDataStruct`.
template<typename T> concept not_void = not std::same_as<void, T>;

/// Check that type `T` seems to be a MIMU data vector. Used in the definition of `MimuDataStruct`.
template<typename T> concept vec3_like
    =  not_void<vecx_t<T>>
    && not_void<vecy_t<T>>
    && not_void<vecz_t<T>>
    && std::same_as<vecx_t<T>, vecy_t<T>>
    && std::same_as<vecy_t<T>, vecz_t<T>>
    ;
/// Check that the type `T` has accelerometer vector data at one of the expected interfaces
template<typename T> concept has_accl = not_void<accl_t<T>> && vec3_like<accl_t<T>>;

/// Check that the type `T` has gyroscope vector data at one of the expected interfaces
template<typename T> concept has_gyro = not_void<gyro_t<T>> && vec3_like<gyro_t<T>>;

/// Check that the type `T` has magnetometer vector data at one of the expected interfaces
template<typename T> concept has_magn = not_void<magn_t<T>> && vec3_like<magn_t<T>>;

/// Check that the type `T` has accelerometer, gyroscope, and magnetometer data at one of the expected interfaces
template<typename T>
concept MimuDataStruct = has_accl<T> && has_gyro<T> && has_magn<T>;

/// Check that the type T has an `outputs` struct that is a MIMU data structure whose vectors satisfy the constraints of `ClearableFlag`
template<typename T>
concept MimuComponent
    =  MimuDataStruct<outputs_t<T>>
    && ClearableFlag<accl_t<outputs_t<T>>>
    && ClearableFlag<gyro_t<outputs_t<T>>>
    && ClearableFlag<magn_t<outputs_t<T>>>
    ;

/// Access the accelerometer data of a MIMU component
auto& accl_of(MimuComponent auto& mimu) { return accl_of(mimu.outputs); }
/// Access the gyroscope data of a MIMU component
auto& gyro_of(MimuComponent auto& mimu) { return gyro_of(mimu.outputs); }
/// Access the magnetometer data of a MIMU component
auto& magn_of(MimuComponent auto& mimu) { return magn_of(mimu.outputs); }

/// Access the first vector component of the accelerometer data of a MIMU data structure or MIMU component
auto accl_x(auto& mimu) { return vecx_of(accl_of(mimu)); }
/// Access the second vector component of the accelerometer data of a MIMU data structure or MIMU component
auto accl_y(auto& mimu) { return vecy_of(accl_of(mimu)); }
/// Access the third vector component of the accelerometer data of a MIMU data structure or MIMU component
auto accl_z(auto& mimu) { return vecz_of(accl_of(mimu)); }

/// Access the first vector component of the gyroscope data of a MIMU data structure or MIMU component
auto gyro_x(auto& mimu) { return vecx_of(gyro_of(mimu)); }
/// Access the second vector component of the gyroscope data of a MIMU data structure or MIMU component
auto gyro_y(auto& mimu) { return vecy_of(gyro_of(mimu)); }
/// Access the third vector component of the gyroscope data of a MIMU data structure or MIMU component
auto gyro_z(auto& mimu) { return vecz_of(gyro_of(mimu)); }

/// Access the first vector component of the magnetometer data of a MIMU data structure or MIMU component
auto magn_x(auto& mimu) { return vecx_of(magn_of(mimu)); }
/// Access the second vector component of the magnetometer data of a MIMU data structure or MIMU component
auto magn_y(auto& mimu) { return vecy_of(magn_of(mimu)); }
/// Access the third vector component of the magnetometer data of a MIMU data structure or MIMU component
auto magn_z(auto& mimu) { return vecz_of(magn_of(mimu)); }

}
