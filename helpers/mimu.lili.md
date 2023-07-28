# MIMU Endpoint Helpers

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
template< string_literal name
        , typename T = float
        , num_literal<T> min = -1.0f
        , num_literal<T> max = 1.0f
        , num_literal<T> init = 0.0f
        , string_literal unit = "normalized"
        , string_literal desc = ""
        , typename ... Tags
        >
struct vec3_message
: array_message<name, 3, desc, T, min, max, init, Tags...>
, unit_<unit>
{
    using Parent = array_message<name, 3, desc, T, min, max, init, Tags...>;
    using Parent::operator=;
    constexpr auto& x() noexcept { return Parent::state[0]; }
    constexpr auto& y() noexcept { return Parent::state[1]; }
    constexpr auto& z() noexcept { return Parent::state[2]; }
    constexpr const auto& x() const noexcept { return Parent::state[0]; }
    constexpr const auto& y() const noexcept { return Parent::state[1]; }
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
// @#'mimu.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "helpers/metadata.hpp"
#include "helpers/endpoints.hpp"

namespace sygaldry {
@{vec3_message}
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
#include "helpers/mimu.hpp"

using namespace sygaldry;

@{tests}
// @/
```

```cmake
# @#'tests/mimu/CMakeLists.txt'
add_executable(mimu-helper-tests tests.cpp)
target_link_libraries(mimu-helper-tests PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(mimu-helper-tests)
# @/
```
