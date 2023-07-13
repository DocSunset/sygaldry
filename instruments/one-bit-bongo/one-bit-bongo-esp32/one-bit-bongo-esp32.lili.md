# ESP32 One Bit Bongo

## Implementation

```cpp
// @#'main/one-bit-bongo.cpp'
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <concepts/runtime.hpp>
#include <components/esp32/button.hpp>
#include <bindings/esp32/spiffs.hpp>
#include <bindings/esp32/wifi.hpp>
#include <bindings/liblo.hpp>
#include <bindings/cli/cli.hpp>
#include <bindings/output_logger.hpp>

using namespace sygaldry;

struct OneBitBongo
{
    struct Instrument
    {
        bindings::esp32::WiFi<bindings::CstdioLogger> wifi;
        components::esp32::Button<GPIO_NUM_23> button;
        bindings::LibloOsc<decltype(button)> osc;
    };

    bindings::esp32::SpiffsSessionStorage<Instrument> session_storage;
    Instrument instrument;
    bindings::CstdioOutputLogger<Instrument> log;
    bindings::CstdioCli<Instrument> cli;
} constinit bongo{};

constexpr auto runtime = Runtime{bongo};

extern "C" void app_main(void)
{
    runtime.init();
    for (;;)
    {
        runtime.tick();
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

set(EXTRA_COMPONENT_DIRS ${SYGALDRY_ROOT}/components/esp32)
project(one-bit-bongo)
# @/
```

Note that we add the ESP32 components directory as `EXTRA_COMPONENT_DIRS` so
that IDF components (such as the `libmapper-arduino` component) will be picked
up by the build system.

We add `sygaldry` as an include directory.

```cmake
# @#'main/CMakeLists.txt'
idf_component_register(SRCS "one-bit-bongo.cpp"
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/pfr/include
        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/mp11/include
        INCLUDE_DIRS "." ${SYGALDRY_ROOT}
        )
# @/
```

