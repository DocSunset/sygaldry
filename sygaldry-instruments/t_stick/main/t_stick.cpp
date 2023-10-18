/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sygse-button.hpp"
#include "sygse-adc.hpp"
#include "sygsa-two_wire.hpp"
#include "sygse-trill.hpp"
#include "sygsp-icm20948.hpp"
#include "sygsa-icm20948-two_wire_serif.hpp"
#include "sygsp-complementary_mimu_fusion.hpp"
#include "sygbe-runtime.hpp"

using namespace sygaldry;

struct TStick
{
    sygse::Button<GPIO_NUM_15> button;
    sygse::OneshotAdc<33> adc;
    sygsa::TrillCraft touch;
    sygsp::ICM20948< sygsa::ICM20948TwoWireSerif<sygsp::ICM20948_I2C_ADDRESS_0>
                   , sygsa::ICM20948TwoWireSerif<sygsp::AK09916_I2C_ADDRESS>
                   > mimu;
    sygsp::ComplementaryMimuFusion<decltype(mimu)> mimu_fusion;
};

sygbe::ESP32Instrument<TStick> tstick{};
extern "C" void app_main(void) { tstick.app_main(); }
