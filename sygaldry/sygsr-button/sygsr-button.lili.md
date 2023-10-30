\page page-sygsr-button sygsr-button: Raspberry Pi Pico SDK Button

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Introductory prose goes here.

Remember to add your new component to the top level CMakeLists.txt!

I repeat: Remember to add your new component to the top level CMakeLists.txt!

```cpp
// @#'sygsr-button.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygsp-button.hpp"

namespace sygaldry { namespace sygsr {

/// \addtogroup sygsr Raspberry Pi Pico SDK Sensors
/// \{

/// \defgroup sygsr-button sygsr-button: Raspberry Pi Pico SDK Button
/// Literate source code: page-sygsr-button
/// \{

/*! \brief Raspberry Pi Pico SDK Button Component

\tparam pin_number The GPIO pin number on which to read the button
\tparam active_level Whether the button is active high or low.
*/
template<unsigned int pin_number, sygsp::ButtonActive active_level = sygsp::ButtonActive::Low>
struct Button : sygsp::ButtonGestureModel
, name_<"Button">
, description_<"A single button attached to a GPIO">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    void init()
    {
        gpio_init(pin_number);
        gpio_set_dir(pin_number, false); // input mode
        if constexpr (active_level == sygsp::ButtonActive::Low) gpio_pull_up(pin_number);
        else gpio_pull_down(pin_number);
    }

    void operator()()
    {
        inputs.button_state = (char)gpio_get(pin_number) == (char)active_level;
        ButtonGestureModel::operator()();
    }
};

/// \}
/// \}

} }
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsr-button)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygah-metadata
        INTERFACE sygsp-button
        INTERFACE hardware_gpio
        )
# @/
```
