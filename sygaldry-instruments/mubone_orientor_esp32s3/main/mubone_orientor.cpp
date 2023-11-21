/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygsa-two_wire.hpp"
#include "sygsp-icm20948.hpp"
#include "sygsa-two_wire_serif.hpp"
#include "sygsp-complementary_mimu_fusion.hpp"
#include "sygbe-runtime.hpp"

using namespace sygaldry;

struct Orientor
{
    sygsp::ICM20948< sygsa::TwoWireByteSerif<sygsp::ICM20948_I2C_ADDRESS_1>
                   , sygsa::TwoWireByteSerif<sygsp::AK09916_I2C_ADDRESS>
                   > mimu;
    sygsp::ComplementaryMimuFusion<decltype(mimu)> mimu_fusion;
};

sygbe::ESP32Instrument<Orientor> orientor{};
extern "C" void app_main(void) { orientor.app_main(); }
