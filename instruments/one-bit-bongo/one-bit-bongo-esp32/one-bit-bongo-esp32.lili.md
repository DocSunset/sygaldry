# ESP32 One Bit Bongo

## Implementation

```cpp
// @#'main/one-bit-bongo.cpp'
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <concepts/runtime.hpp>
#include <components/esp32/button.hpp>
#include <bindings/cli/cli.hpp>
#include <bindings/output_logger.hpp>
#include <bindings/esp32/wifi.hpp>

using namespace sygaldry;

struct OneBitBongo
{
    struct api_t
    {
        bindings::esp32::WiFi<bindings::CstdioLogger> wifi;
        components::esp32::Button<GPIO_NUM_23> button;
    } api;

    bindings::CstdioOutputLogger<decltype(api)> log;
    bindings::CstdioCli<decltype(api)> cli;
} constinit bongo{};

constexpr auto runtime = Runtime{bongo};

extern "C" void app_main(void)
{
    init(bongo);
    for (;;)
    {
        clear_output_flags(bongo.api.button);
        bongo.api.button();
        bongo.log(bongo.api);
        bongo.cli(bongo.api);
        clear_input_flags(bongo.api.button);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
// @/
```

## Build Boilerplate

The `idf.py init` generated boilerplate `CMakeLists.txt` for an `esp-idf` project:

```cmake
# @#'CMakeLists.txt'
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(one-bit-bongo)
# @/

We add `sygaldry` as an include directory.

# @#'main/CMakeLists.txt'
idf_component_register(SRCS "one-bit-bongo.cpp"
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/pfr/include
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/mp11/include
        INCLUDE_DIRS "." ${SYGALDRY_ROOT}
        )
# @/
```
