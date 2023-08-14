/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "sygaldry-concepts-mimu.hpp"
#include "sygaldry-helpers-mimu.hpp"

using namespace sygaldry;

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

