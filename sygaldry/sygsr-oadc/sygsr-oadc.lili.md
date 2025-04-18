\page page-sygsr-oadc sygsr-oadc: Raspberry Pi Pico SDK Oversampling ADC Driver

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: SPDX-License-Identifier: MIT

[TOC]

Analog-digital converter component that takes multiple measures and returns
their average.

```cpp
// @#'sygsr-oadc.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"
#include <hardware/adc.h>

namespace sygaldry { namespace sygsr {

/// \addtogroup sygsr
/// \{

/// \defgroup sygsr-oadc sygsr-oadc: Raspberry Pi Pico SDK ADC Driver
/// Literate source code: page-sygsr-oadc
/// \{

/*! \defgroup sygsr-oadc-channels sygsr-oadc: ADC Channels

The internal temperature sensors is currently not supported through this
component.
*/
/// \{
static constexpr unsigned int OADC_CHANNEL_0 = 0;
static constexpr unsigned int OADC_CHANNEL_1 = 1;
static constexpr unsigned int OADC_CHANNEL_2 = 2;
static constexpr unsigned int OADC_CHANNEL_3 = 3;
static constexpr unsigned int OADC_GPIO[] = {26,27,28,29};
/// \}

/*! \brief Oneshot analog-digital converter
*/
template<unsigned int input_number, unsigned int oversampling_factor, unsigned int skip_factor = 0>
struct OversamplingAdc
: name_<"OADC">
, description_<"Oversampling analog-digital converter">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    static_assert(OADC_CHANNEL_0 <= input_number && input_number <= OADC_CHANNEL_3);

    struct outputs_t {
        slider<"raw", "average of multiple raw ADC measurements"
        , float, 0, 4096, 0
        > raw;
    } outputs;

    void init()
    {
        adc_init();
        adc_gpio_init(OADC_GPIO[input_number]);
    }

    void main()
    {
        adc_select_input(input_number);
        outputs.raw = 0;
        for (unsigned int n = 1; n < oversampling_factor+1; ++n)
        {
            if (n <= skip_factor) {adc_read(); continue;}
            outputs.raw += ((float)adc_read() - outputs.raw) / (n-skip_factor);
        }
    }
};

/// \}
/// \}

} }
// @/
```

```cpp
// @#'sygsr-oadc.test.cpp'
#include <catch2/catch_test_macros.hpp>
#include "sygsr-oadc.hpp"

using namespace sygaldry;
using namespace sygaldry::sygsr;

// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsr-oadc)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-endpoints
        INTERFACE sygah-metadata
        INTERFACE hardware_adc
        )

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
#target_link_libraries(${lib}-test PRIVATE OTHERREQUIREDPACKAGESANDCOMPONENTSHERE)
catch_discover_tests(${lib}-test)
endif()
# @/
```
