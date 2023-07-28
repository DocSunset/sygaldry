#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <utility>
#include "concepts/components.hpp"

namespace sygaldry {

#define try_spelling(SPELLING) if constexpr (requires {mimu_data.SPELLING;}) return mimu_data.SPELLING

auto& accl_of(auto& mimu_data)
{
    try_spelling(accelerometer);
    else try_spelling(acceleration);
    else try_spelling(accel);
    else try_spelling(accl);
    else try_spelling(acc);
    else try_spelling(a);
    else return;
}

auto& gyro_of(auto& mimu_data)
{
    try_spelling(gyroscope);
    else try_spelling(angular_rate);
    else try_spelling(gyro);
    else try_spelling(g);
    else return;
}

auto& magn_of(auto& mimu_data)
{
    try_spelling(magnetometer);
    else try_spelling(magnetic_field);
    else try_spelling(magn);
    else try_spelling(mag);
    else try_spelling(m);
    else return;
}

#undef try_spelling

template<typename T> using accl_t = std::remove_cvref_t<decltype(accl_of(std::declval<std::remove_cvref_t<T>&>()))>;
template<typename T> using gyro_t = std::remove_cvref_t<decltype(gyro_of(std::declval<std::remove_cvref_t<T>&>()))>;
template<typename T> using magn_t = std::remove_cvref_t<decltype(magn_of(std::declval<std::remove_cvref_t<T>&>()))>;

template<typename T> concept not_void = not std::same_as<void, T>;

template<typename T> concept has_accl = not_void<accl_t<T>>;
template<typename T> concept has_gyro = not_void<gyro_t<T>>;
template<typename T> concept has_magn = not_void<magn_t<T>>;

template<typename T>
concept Mimu = has_accl<T> && has_gyro<T> && has_magn<T>;
template<typename T>
concept MimuComponent
    =  Mimu<outputs_t<T>>
    && ClearableFlag<accl_t<outputs_t<T>>>
    && ClearableFlag<gyro_t<outputs_t<T>>>
    && ClearableFlag<magn_t<outputs_t<T>>>
    ;

}
