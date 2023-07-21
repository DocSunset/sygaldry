/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>
#include <components/esp32/tests/gpio.hpp>
#include <components/esp32/tests/button.hpp>

extern "C" void app_main(void)
{
    printf("Type g to run tests.\n");

    while ('g' != getchar()) vTaskDelay(50 / portTICK_PERIOD_MS);

    vTaskDelay(100 / portTICK_PERIOD_MS); // give time for test host to connect

    UNITY_BEGIN();
    RUN_TEST(gpio);
    RUN_TEST(button);
    UNITY_END();
}
