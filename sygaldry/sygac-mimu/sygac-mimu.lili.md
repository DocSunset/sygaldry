\page page-sygac-mimu MIMU Concepts

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes concepts useful for implementing MIMU components.

# Endpoints Existence

A MIMU is a component that exposes output endpoints for the accelerometer,
gyroscope, and magnetometer. Each of these are detected with their own concepts
so that different spellings are possible. We define the concept in terms of the
generic accessor function; if none of the expected spellings are found, the
accessor returns void. The concept returns true for any type where the accessor
does not return void. We use a macro to facilitate the implementation of
the accessors.

As well as existing, the MIMU data needs to be accessible somehow. A number of
different APIs are supported and abstracted over, including array subscript
operator, x-y-z methods, and x-y-z members.

```cpp
// @='endpoints'
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
// @/
```

With these generic functions for accessing the elements of a MIMU data vector,
and for accessing a MIMU data vector from a MIMU data structure, we go on to
define type traits and concepts. The type traits are a bit of a mouthful, but
it essentially boils down to `decltype(accl_of(T))` modulo `detail`, `declval`
and removed and added references. It's an implementation detail. Don't worry
too much about it unless you're trying to fix it...

```cpp
// @+'endpoints'
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
// @/
```

We define an intermediate concept that checks if a type has an x, y, and z
component that are all the same type.

```cpp
// @+'endpoints'
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
// @/
```

And then the main concepts start to fall into place.

```cpp
// @+'endpoints'
/// Check that the type `T` has accelerometer vector data at one of the expected interfaces
template<typename T> concept has_accl = not_void<accl_t<T>> && vec3_like<accl_t<T>>;

/// Check that the type `T` has gyroscope vector data at one of the expected interfaces
template<typename T> concept has_gyro = not_void<gyro_t<T>> && vec3_like<gyro_t<T>>;

/// Check that the type `T` has magnetometer vector data at one of the expected interfaces
template<typename T> concept has_magn = not_void<magn_t<T>> && vec3_like<magn_t<T>>;

/// Check that the type `T` has accelerometer, gyroscope, and magnetometer data at one of the expected interfaces
template<typename T>
concept MimuDataStruct = has_accl<T> && has_gyro<T> && has_magn<T>;
// @/
```

```cpp
// @+'tests'
struct MimuIsh {
    struct outputs_t {
        struct accl_t {float x = 0; float y; float z;} accl;
        struct gyro_t {float x = 1; float y; float z;} gyro;
        struct magn_t {float x = 2; float y; float z;} magn;
    } outputs;
} mimuish;

static_assert(has_accl<decltype(mimuish.outputs)>);
static_assert(has_gyro<decltype(mimuish.outputs)>);
static_assert(has_magn<decltype(mimuish.outputs)>);
// @/
```

# MIMU Components

The above concepts are the most general possible. Sygaldry imposes a few
additional expectations on MIMU data for its sensor fusion and mapping
facilities to be able to injest this data.

First, a MIMU component is one whose output endpoints structure meets the above
`MimuDataStruct` concept. Second, MIMU data within Sygaldry must signal that it
has been updated, meeting the requirements of `ClearableFlag`. This gives the
following concept:

```cpp
// @+'MimuDataStruct'
/// Check that the type T has an `outputs` struct that is a MIMU data structure whose vectors satisfy the constraints of `ClearableFlag`
template<typename T>
concept MimuComponent
    =  MimuDataStruct<outputs_t<T>>
    && ClearableFlag<accl_t<outputs_t<T>>>
    && ClearableFlag<gyro_t<outputs_t<T>>>
    && ClearableFlag<magn_t<outputs_t<T>>>
    ;
// @/
```

# Accessors

We define the following additional accessors for convenience.

```cpp
// @+'MIMU accessors'
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
// @/
```

```cpp
// @+'tests'
struct TestMimuComponent {
    struct outputs_t {
        vec3_message<"accl"> accl;
        vec3_message<"gyro"> gyro;
        vec3_message<"magn"> magn;
    } outputs;
} testmimu;

static_assert(MimuComponent<TestMimuComponent>);

TEST_CASE("mimu endpoints accessors")
{
    CHECK(accl_of(mimuish.outputs).x == 0);
    CHECK(gyro_of(mimuish.outputs).x == 1);
    CHECK(magn_of(mimuish.outputs).x == 2);
    CHECK(accl_x(mimuish.outputs) == 0);
    CHECK(gyro_x(mimuish.outputs) == 1);
    CHECK(magn_x(mimuish.outputs) == 2);
    CHECK(accl_x(testmimu) == 0);
    CHECK(gyro_x(testmimu) == 0);
    CHECK(magn_x(testmimu) == 0);
}
// @/
```

# Summary

```cpp
// @#'sygac-mimu.hpp'
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

@{endpoints}

@{MimuDataStruct}

@{MIMU accessors}

}
// @/

// @#'sygac-mimu.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "sygah-mimu.hpp"
#include "sygac-mimu.hpp"

using namespace sygaldry;

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygac-mimu)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygac-components)
target_link_libraries(sygac INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
target_link_libraries(${lib}-test PRIVATE sygah)
catch_discover_tests(${lib}-test)
endif()
# @/
```
