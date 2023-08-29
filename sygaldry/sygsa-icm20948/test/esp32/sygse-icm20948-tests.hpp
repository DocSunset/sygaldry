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
#include "sygsa-icm20948-two_wire_serif.hpp"
#include "sygsa-icm20948_registers.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace sygaldry { namespace sygse {

template<uint8_t i2c_address>
struct ICM20948Tests
{
    using Serif = sygsa::ICM20948TwoWireSerif<i2c_address>;
    using Registers = ICM20948Registers<Serif>;
    static void test()
    {
        {
            printf("icm20948-test: read test... ");
            constexpr uint8_t who_am_i_address = 0;
            constexpr uint8_t who_am_i_value = 0xea;
            uint8_t ret = Serif::read(who_am_i_address);
            if (ret == who_am_i_value) printf("passed!\n");
            else
            {
                printf("unexpected who am I value?!\n");
                return;
            }
        }
        {
            printf("icm20948-test: write test...");

            constexpr uint8_t pwr_mgmt_1_address = 6;
            constexpr uint8_t expected_value_after_reset = 0x41;
            constexpr uint8_t wake_up_value = 0x01;
            constexpr uint8_t reset_trigger_value = 0x81;

            // Reset device
            printf(".");
            Serif::write(pwr_mgmt_1_address, reset_trigger_value);
            vTaskDelay(pdMS_TO_TICKS(10));

            // Read USER_CTRL register after reset
            uint8_t value_after_reset = Serif::read(pwr_mgmt_1_address);
            if (value_after_reset != expected_value_after_reset)
            {
                printf(" unexpected value after reset?!\n");
                return;
            }

            // Toggle a switch in USER_CTRL register
            printf(".");
            Serif::write(pwr_mgmt_1_address, wake_up_value);
            vTaskDelay(pdMS_TO_TICKS(10));

            // Confirm the change was recorded
            uint8_t value_after_wake_up = Serif::read(pwr_mgmt_1_address);
            if (value_after_wake_up != wake_up_value)
            {
                printf(" write operation unsuccessful?!\n");
                return;
            }

            // Reset device
            printf(".");
            Serif::write(pwr_mgmt_1_address, reset_trigger_value);
            vTaskDelay(pdMS_TO_TICKS(10));

            printf(" passed!\n");
        }
        {
            printf("icm20948-test: register bases test...");

            if (Registers::WHO_AM_I::read() != Registers::WHO_AM_I::after_reset)
            {
                printf("unexpected who am I value?!\n");
                return;
            }

            printf(".");
            Registers::PWR_MGMT_1::DEVICE_RESET::trigger();
            vTaskDelay(pdMS_TO_TICKS(10));

            if (Registers::PWR_MGMT_1::read() != Registers::PWR_MGMT_1::after_reset)
            {
                printf(" unexpected value after reset?!\n");
                return;
            }

            if (Registers::PWR_MGMT_1::read() != Registers::PWR_MGMT_1::after_reset)
            {
                printf(" unexpected value after reset?!\n");
                return;
            }

            printf(".");
            Registers::PWR_MGMT_1::SLEEP::disable();
            vTaskDelay(pdMS_TO_TICKS(10));

            // Confirm the change was recorded
            uint8_t value_after_wake_up = Registers::PWR_MGMT_1::read();
            uint8_t expected = Registers::PWR_MGMT_1::after_reset & ~Registers::PWR_MGMT_1::SLEEP::mask;
            if (value_after_wake_up != expected)
            {
                printf(" write operation unsuccessful?!\n");
                return;
            }

            printf(".");
            PWR_MGMT_1::DEVICE_RESET::trigger();
            vTaskDelay(pdMS_TO_TICKS(10));

            printf(" passed!\n");
        }
    }
};

} }
