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

/// \addtogroup sygsp-icm20948
/// \{

template<typename Serif, typename AK09916Serif>
struct ICM20948Tests
{
    using Registers =        ICM20948Registers<Serif>;
    using AK09916Registers = ICM20948Registers<AK09916Serif>;
    static bool test()
    {
        {
            printf("icm20948-test: read test... ");
            constexpr uint8_t who_am_i_address = 0;
            constexpr uint8_t who_am_i_value = 0xea;
            uint8_t ret = Serif::read(who_am_i_address);
            if (ret == who_am_i_value) printf("passed!\n");
            else
            {
                printf("unexpected who am I value %d?!\n", ret);
                return false;
            }
        }
        {
            printf("icm20948-test: write test...");

            constexpr uint8_t pwr_mgmt_1_address = 6;
            constexpr uint8_t expected_value_after_reset = 0x41;
            constexpr uint8_t wake_up_value = 0x01;
            constexpr uint8_t reset_trigger_value = 0x81;

            // Reset device
            Serif::write(pwr_mgmt_1_address, reset_trigger_value);
            delay(10);

            // Read USER_CTRL register after reset
            uint8_t value_after_reset = Serif::read(pwr_mgmt_1_address);
            if (value_after_reset != expected_value_after_reset)
            {
                printf(" unexpected value after reset?!\n");
                return false;
            }

            // Toggle a switch in USER_CTRL register
            printf(".");
            Serif::write(pwr_mgmt_1_address, wake_up_value);
            delay(10);

            // Confirm the change was recorded
            uint8_t value_after_wake_up = Serif::read(pwr_mgmt_1_address);
            if (value_after_wake_up != wake_up_value)
            {
                printf(" write operation unsuccessful?!\n");
                return false;
            }

            // Reset device
            printf(".");
            Serif::write(pwr_mgmt_1_address, reset_trigger_value);
            delay(10);

            printf(" passed!\n");
        }
        {
            printf("icm20948-test: register bases test...\n");

            if (Registers::WHO_AM_I::read() != Registers::WHO_AM_I::after_reset)
            {
                printf("    unexpected who am I value?!\n");
                return false;
            }
            printf("    WHO_AM_I good...\n");

            Registers::PWR_MGMT_1::DEVICE_RESET::trigger();
            delay(1);

            auto v = Registers::PWR_MGMT_1::read();
            if (v != Registers::PWR_MGMT_1::after_reset)
            {
                printf("    unexpected value %x after reset?!\n", v);
                return false;
            }
            printf("    value after reset good...\n");

            Registers::PWR_MGMT_1::SLEEP::disable();
            delay(1);

            // Confirm the change was recorded
            uint8_t value_after_wake_up = Registers::PWR_MGMT_1::read();
            uint8_t expected = Registers::PWR_MGMT_1::after_reset & ~Registers::PWR_MGMT_1::SLEEP::mask;
            if (value_after_wake_up != expected)
            {
                printf("    write operation unsuccessful?!\n");
                return false;
            }
            printf("    write operation good...\n");

            Registers::PWR_MGMT_1::DEVICE_RESET::trigger();
            delay(1);

            printf("   passed!\n");
        }
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
            AK09916Registers::CNTL3::SRST::trigger(); delay(1);
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
                printf("x data %d outside self-test range?! x: %d  y: %d  z: %d\n", x, x, y, z);
                return false;
            }
            if (!(-200 <= y && y <= 200))
            {
                printf("y data %d outside self-test range?! x: %d  y: %d  z: %d\n", y, x, y, z);
                return false;
            }
            if (!(-1000 <= z && z <= -200))
            {
                printf("z data %d outside self-test range?! x: %d  y: %d  z: %d\n", z, x, y, z);
                return false;
            }
            printf("AK09916 self test pass! x: %d  y: %d  z: %d\n", x, y, z);
        }
        return true;
    }
};

/// \}
} }
