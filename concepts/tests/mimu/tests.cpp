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
static_assert(Mimu<MimuIsh::outputs_t>);
