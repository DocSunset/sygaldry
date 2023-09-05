/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>
#include <cstdio>
#include "sygsp-icm20948_registers.hpp"
#include "sygsp-delay.hpp"

namespace sygaldry { namespace sygsp {

template<typename Serif, typename AK09916Serif>
struct ICM20948Tests
{
    using Registers =        ICM20948Registers<Serif>;
    using AK09916Registers = ICM20948Registers<AK09916Serif>;
    static bool test()
    {
        {
            printf("icm20948-test: AK09916 test... \n");
            Registers::PWR_MGMT_1::DEVICE_RESET::trigger(); delay(10);
            Registers::PWR_MGMT_1::SLEEP::disable(); delay(10);
            Registers::PWR_MGMT_1::LP_EN::disable(); delay(1);
            Registers::INT_PIN_CFG::BYPASS_EN::enable(); delay(1);
            Registers::USER_CTRL::I2C_MST_EN::disable(); delay(1);

            if (AK09916Registers::WIA2::read() != AK09916Registers::WIA2::after_reset)
            {
                printf("unable to connect to AK09916\n");
                return false;
            }
            else printf("AK09916 connected...\n");
            AK09916Registers::CNTL2::MODE::PowerDown::set(); delay(1);
            AK09916Registers::CNTL2::MODE::SelfTest::set(); delay(1);
            int i = 0;
            while (i < 100 && !AK09916Registers::ST1::DRDY::read_field()) { delay(10); }
            if (i >= 100)
            {
                printf("timeout while waiting for data ready?!\n");
                return false;
            }
            constexpr uint8_t N_OUT = 8;
            uint8_t measurement_data[N_OUT] = {0};
            AK09916Serif::read(AK09916Registers::HXL::address, measurement_data, N_OUT);
            int16_t x = measurement_data[1] << 8 | (measurement_data[0] & 0xFF);
            int16_t y = measurement_data[3] << 8 | (measurement_data[2] & 0xFF);
            int16_t z = measurement_data[5] << 8 | (measurement_data[4] & 0xFF);
            if (!(-200 <= x && x <= 200))
            {
                printf("x data %d outside self-test range?!\n", x);
                return false;
            }
            if (!(-200 <= y && y <= 200))
            {
                printf("y data %d outside self-test range?!\n", y);
                return false;
            }
            if (!(-1000 <= z && z <= -200))
            {
                printf("z data %d outside self-test range?!\n", z);
                return false;
            }
        }
        return true;
    }
};

} }
