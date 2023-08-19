\page page-sygah-mimu MIMU Endpoint Helpers

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes the endpoint helpers intended to be used in the
implementation of MIMU components. A MIMU is any component that has the
following endpoints:

```cpp
struct outputs_t {
    vec3_message<"accelerometer"> accl; /* or acceleration or accelerometer or accel or a */
    vec3_message<"gyroscope"> gyro;     /* or gyroscope or angular_rate or g */
    vec3_message<"magnetometer"> magn;  /* or magnetometer or magnetic_field or m */
} outputs;
```

# vec3_message

MIMU sensors are generally measured synchronously, but it's not uncommon for
the accelerometer, gyroscope, and magnetometer to have different sampling
rates. For this reason, we employ the `array_message` endpoint helper so that
updates to each sensor can be recognized by converting the endpoint to a
boolean. We then augment this base endpoint with methods for vector-style
access `x(), y(), z()`, and a unit of measurement metadata string.

```cpp
// @+'vec3_message'
/*! \brief A MIMU data vector
\tparam name The name of the vector, e.g. "accelerometer"
\tparam T The underlying numeric type, usually float or int
\tparam min The expected minimum magnitude of the vector
\tparam max The expected maximum magnitude of the vector
\tparam unit The unit of measurement of the vector
\tparam desc A textual description of the vector
\tparam Tags A list of tag helper classes to apply to the endpoint
*/
template< string_literal name
        , typename T = float
        , num_literal<T> min = -1.0f
        , num_literal<T> max = 1.0f
        , string_literal unit = "normalized"
        , string_literal desc = ""
        , typename ... Tags
        >
struct vec3_message
: array_message<name, 3, desc, T, min, max, T{}, Tags...>
, unit_<unit>
{
    /// The parent type; this is defined as a convenience for implementation and shouldn't be treated as part of the public API
    using Parent = array_message<name, 3, desc, T, min, max, T{}, Tags...>;
    using Parent::operator=;
    /// Mutable vector component access; remember to call `set_updated()` if you change the value of the vector.
    constexpr auto& x() noexcept { return Parent::state[0]; }
    /// Mutable vector component access; remember to call `set_updated()` if you change the value of the vector.
    constexpr auto& y() noexcept { return Parent::state[1]; }
    /// Mutable vector component access; remember to call `set_updated()` if you change the value of the vector.
    constexpr auto& z() noexcept { return Parent::state[2]; }
    /// Immutable vector component access
    constexpr const auto& x() const noexcept { return Parent::state[0]; }
    /// Immutable vector component access
    constexpr const auto& y() const noexcept { return Parent::state[1]; }
    /// Immutable vector component access
    constexpr const auto& z() const noexcept { return Parent::state[2]; }
};
// @/
```
Note that, due to the semantics of `array_message`, only updates via the
assignment operator will set the `updated` flag of the endpoint. Changes
made via the array access and vector access methods must be followed with
a manual call to `set_updated()`.

```cpp
// @+'tests'
TEST_CASE("MIMU endpoint helpers")
{
    vec3_message<"testvec"> v{};
    CHECK(not bool(v));
    CHECK(v.x() == 0.0f);
    CHECK(v.y() == 0.0f);
    CHECK(v.z() == 0.0f);
    v.x() = 1.0f;
    CHECK(not bool(v)); // update through accessor doesn't set updated flag
    v = {1.0f, 2.0f, 3.0f}; // update by operator= instead!
    CHECK(bool(v));
    CHECK(v.x() == 1.0f);
    CHECK(v.y() == 2.0f);
    CHECK(v.z() == 3.0f);
}

struct TestMimu
{
    struct outputs_t {
        vec3_message<"accl"> accl;
        vec3_message<"gyro"> gyro;
        vec3_message<"magn"> magn;
    } outputs;
};
static_assert(MimuComponent<TestMimu>);

// @/
```

# Summary

```cpp
// @#'sygah-mimu.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry {
///\defgroup helpers_mimu_endpoints MIMU Endpoints Helpers
///\{
@{vec3_message}
///\}
}
// @/

// @#'sygah-mimu.test.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "sygac-mimu.hpp"
#include "sygah-mimu.hpp"

using namespace sygaldry;

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygah-mimu)
add_library(${lib} INTERFACE)
target_link_libraries(${lib} INTERFACE sygah-metadata)
target_link_libraries(${lib} INTERFACE sygah-endpoints)
target_include_directories(${lib} INTERFACE .)

target_link_libraries(sygah INTERFACE ${lib})

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain
        PRIVATE sygah-mimu
        PRIVATE sygac-mimu
        )
catch_discover_tests(${lib}-test)
endif()
# @/
```
