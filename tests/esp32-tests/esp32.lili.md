# ESP32 Component Tests

Tests for ESP32 are compiled as ESP32 components using the native toolchain.
This document sets up the boilerplate necessary to compile and detect these
tests.

ESP-IDF provides a unit testing app based on its component structure, but due
to the way the framework is installed on my system, it's a PITA to use, requiring
super user priviledges, and generally not meeting my requirements. For this
reason, the given method is eschewed, which means we have to come up with our
own way of running tests on the device.

For now we make a plain old ESP32 project that just happens to run our tests in
its `app_main` subroutine. This requires a frustrating amount of bookkeeping:
we need to manually include all of the tests *and* remember to then run them,
as well as ensuring that all their required ESP-IDF components are added to
the `REQUIRES` list of the test runner IDF component.

```cmake
# @#'CMakeLists.txt'
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(esp-32-tests)
# @/

# @#'main/CMakeLists.txt'
idf_component_register(SRCS "test_runner.cpp"
        INCLUDE_DIRS ${SYGALDRY_ROOT}
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/pfr/include
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/mp11/include
        REQUIRES unity driver freertos
        )
# @/
```

```c
// @#'main/test_runner.cpp'
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
// @/
```