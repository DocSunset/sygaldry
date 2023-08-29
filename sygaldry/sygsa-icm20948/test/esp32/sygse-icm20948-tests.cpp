/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygse-icm20948-tests.hpp"
#include "sygsa-icm20948-two_wire_serif.hpp"
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace sygaldry { namespace sygse {

template<uint8_t i2c_address>
void ICM20948Tests<i2c_address>::test()
{
    using Serif = sygsa::ICM20948TwoWireSerif<i2c_address>;
    {
        printf("icm20948-test: read test... ");
        constexpr uint8_t who_am_i_address = 0;
        constexpr uint8_t who_am_i_value = 0xea;
        uint8_t ret = Serif::read(who_am_i_address);
        if (ret == who_am_i_value) printf("passed!\n");
        else printf("unexpected who am I value?!\n");
    }
    {
        printf("icm20948-test: write test...");

        constexpr uint8_t pwr_mgmt_1_address = 6;
        constexpr uint8_t expected_value_after_reset;
        constexpr uint8_t wake_up_value = 0x01;
        constexpr uint8_t reset_trigger_value = 0x81;

        // Reset device
        printf(".");
        Serif::write(pwr_mgmt_1_address, reset_trigger_value);
        vTaskDelay(pdMS_TO_TICKS(10));

        // Read USER_CTRL register after reset
        uint8_t value_after_reset = Serif::read(pwr_mgmt_1_address);
        if (value_after_reset != expected_value_after_reset) printf(" unexpected value after reset?!\n");

        // Toggle a switch in USER_CTRL register
        printf(".");
        Serif::write(pwr_mgmt_1_address, wake_up_value);
        vTaskDelay(pdMS_TO_TICKS(10));

        // Confirm the change was recorded
        uint8_t value_after_wake_up = Serif::read(pwr_mgmt_1_address);
        if (value_after_wake_up != wake_up_value) printf(" write operation unsuccessful?!\n");

        // Reset device
        printf(".");
        Serif::write(pwr_mgmt_1_address, reset_trigger_value);
        vTaskDelay(pdMS_TO_TICKS(10));

        printf(" passed!\n");
    }
}

} }
