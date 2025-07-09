\page page-sygsr-pin-driver-array sygsr-pin-driver-array: PicoSDK Pin Driver Array

Copyright 2024 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: SPDX-License-Identifier: MIT

[TOC]

This component drives a bank of output pins from an array. The component is
designed to accept the array as an argument to its main subroutine, so that an
external component is responsible for populating and manipulating the array.
This array can then be treated by the other component as equivalent to the bank
of output pins.

```cpp
// @#'sygsr-pin-driver-array.hpp'
#pragma once
/*
Copyright 2024 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace sygsr {

/// \addtogroup sygsr
/// \{

/// \defgroup sygsr-pin-driver-array sygsr-pin-driver-array: Pin Driver Array
/// Literate source code: page-sygsr-pin-driver-array
/// \{

/*! \brief Drives a bank of output pins from an array. Template args give array source and GPIO pin mapping.
*/
template<typename array_t, unsigned int ... pin_gpios>
struct PinDriverArray
: name_<"Pin Driver Array">
, description_<"Drives a bank of output pins from an array">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    static constexpr unsigned int pins[] = {pin_gpios...};

    void init()
    {
        for (const auto& pin : pins)
        {
            gpio_init(pin);
            gpio_set_dir(pin, true); // output
        }
    }

    void main(const array_t& a)
    {
        static_assert(sizeof...(pin_gpios) == a.size());
        for (std::size_t i = 0; i < a.size(); ++i)
        {
            if (a[i] != gpio_get_out_level(pins[i]))
                gpio_put(pins[i], a[i]);
        }
    }
};

/// \}
/// \}

} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsr-pin-driver-array)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-endpoints
        INTERFACE sygah-metadata
        INTERFACE hardware_gpio
        )
# @/
```
