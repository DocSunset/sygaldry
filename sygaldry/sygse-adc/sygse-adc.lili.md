\page page-sygse-adc sygse-adc: Oneshot ADC Driver

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

The ADC component provides a very basic wrapper around the one-shot ADC read
API provided by the ESP-IDF; it is based on the
[`oneshot_read_main.c` example](https://github.com/espressif/esp-idf/blob/v5.1-rc1/examples/peripherals/adc/oneshot_read/main/oneshot_read_main.c)
provided with the IDF documentation.

[TOC]

# Implementation

# Summary

```cpp
// @#'sygse-adc.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-endpoints.hpp"
#include "sygah-metadata.hpp"
#include "syghe-pins.hpp"

namespace sygaldry { namespace sygse {

/*! \addtogroup sygse sygse: ESP32 Sensors */
/// \{
/*! \defgroup sygse-adc sygse-adc: Oneshot ADC Driver */
///\{

/// Output endpoints for OneshotAdc
struct OneshotAdcOutputs {
    slider<"raw", "raw binary representation of the analog voltage measured by the ADC"
    , int, 0, 4096, 0 // esp32 technical reference manual V5 section 29.3 says 12 bit max resolution
    > raw;
};

namespace detail {
struct OneshotAdcImpl
{
    using outputs_t = OneshotAdcOutputs;
    void * pimpl;
    void init(int gpio_num, outputs_t& outputs);
    void main(outputs_t& outputs);
};
}

/*! \brief Driver component for the analog-digital converter.

*OneshotAdc* configures analog-digital converter 1 (ADC1) to read in oneshot
mode from the given GPIO pin and polls its current value in each loop.

\tparam gpio_num The GPIO number associated with the channel to read.
                 See \ref sygse-adc-intantiations for
                 guidance on which GPIO number corresponds to which ADC
                 channel.
*/
template<syghe::AdcChannel gpio_num>
struct OneshotAdc
: name_<"ESP32 Oneshot ADC">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
{
    using outputs_t = OneshotAdcOutputs;
    outputs_t outputs;
    detail::OneshotAdcImpl pimpl;

    /// Configure the ADC1 channel for the given gpio_num in oneshot read mode.
    void init() {pimpl.init(gpio_num, outputs);}

    /// Poll the current value of the given ADC1 channel.
    void main() {pimpl.main(outputs);}
};

/// \}
/// \}

} }
// @/
```

```cpp
// @#'sygse-adc.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygse-adc.hpp"
#include <esp_err.h>
#include <esp_adc/adc_oneshot.h>

namespace {
    struct State_ {
        adc_channel_t channel;
        adc_oneshot_unit_handle_t adc_handle;
    };
}

namespace sygaldry { namespace sygse { namespace detail {

void OneshotAdcImpl::init(int gpio_num, outputs_t& outputs)
{
    auto& state = *(new State_{});
    adc_oneshot_unit_init_cfg_t unit_config{};
    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_0,
        .bitwidth = ADC_BITWIDTH_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_io_to_channel(gpio_num, &unit_config.unit_id, &state.channel));
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, &state.adc_handle));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(state.adc_handle, state.channel, &channel_config));

    pimpl = static_cast<void*>(&state);
}

void OneshotAdcImpl::main(outputs_t& outputs)
{
    auto& state = *static_cast<State_*>(pimpl);
    ESP_ERROR_CHECK(adc_oneshot_read(state.adc_handle, state.channel, &outputs.raw.value));
}

} } }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygse-adc)

add_library(${lib} INTERFACE)
target_sources(${lib}
        INTERFACE ${lib}.cpp
        )
target_include_directories(${lib}
        INTERFACE .
        )
target_link_libraries(${lib}
        INTERFACE sygah-endpoints
        INTERFACE sygah-metadata
        INTERFACE syghe-pins
        INTERFACE idf::esp_adc
        INTERFACE idf::esp_common
        )
# @/
```
