/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <catch2/catch_test_macros.hpp>
#include "sygaldry-helpers-mimu.hpp"
#include "sygaldry-concepts-mimu.hpp"

using namespace sygaldry;

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
