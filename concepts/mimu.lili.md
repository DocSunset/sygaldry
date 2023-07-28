# MIMU Concepts

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes concepts useful for implementing MIMU components.

# Endpoints

A MIMU is a component that exposes output endpoints for the accelerometer,
gyroscope, and magnetometer. Each of these are detected with their own concepts
so that different spellings are possible. We define the concept in terms of the
generic accessor function; if none of the expected spellings are found, the
accessor returns void. The concept returns true for any type where the accessor
does not return void. We use a macro to facilitate the implementation of
the accessors.

```cpp
// @='endpoints'
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
// @/

// @+'tests'
struct MimuIsh {
    struct outputs_t {
        struct accl_t {int i = 0;} accl;
        struct gyro_t {int i = 1;} gyro;
        struct magn_t {int i = 2;} magn;
    } outputs;
} mimuish;

TEST_CASE("mimu endpoints concepts")
{
    CHECK(accl_of(mimuish.outputs).i == 0);
    CHECK(gyro_of(mimuish.outputs).i == 1);
    CHECK(magn_of(mimuish.outputs).i == 2);
}

static_assert(has_accl<decltype(mimuish.outputs)>);
static_assert(has_gyro<decltype(mimuish.outputs)>);
static_assert(has_magn<decltype(mimuish.outputs)>);
// @/
```

# MIMU

A MIMU is then simply defined as a container with the appropriate data:

```cpp
// @='Mimu'
template<typename T>
concept Mimu = has_accl<T> && has_gyro<T> && has_magn<T>;
// @/

// @+'tests'
static_assert(Mimu<MimuIsh::outputs_t>);
// @/
```

# MIMU Components

The above concepts a the most general possible. Sygaldry imposes a few
additional expectations on MIMU data for its sensor fusion and mapping
facilities to be able to injest this data.

First, a MIMU component is one whose output endpoints structure meets the above
`Mimu` concept. Second, MIMU data within Sygaldry must signal that it has been
updated, meeting the requirements of `ClearableFlag`. This gives

```cpp
// @+'Mimu'
template<typename T>
concept MimuComponent
    =  Mimu<outputs_t<T>>
    && ClearableFlag<accl_t<outputs_t<T>>>
    && ClearableFlag<gyro_t<outputs_t<T>>>
    && ClearableFlag<magn_t<outputs_t<T>>>
    ;
// @/
```

# Summary

```cpp
// @#'mimu.hpp'
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

@{endpoints}

@{Mimu}

}
// @/

// @#'tests/mimu/tests.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "concepts/mimu.hpp"

using namespace sygaldry;

@{tests}
// @/
```

```cmake
# @#'tests/mimu/CMakeLists.txt'
add_executable(mimu-concepts-tests tests.cpp)
target_link_libraries(mimu-concepts-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(mimu-concepts-tests)
# @/
```
