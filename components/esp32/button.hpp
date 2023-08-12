#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <components/button.hpp>
#include <components/esp32/gpio.hpp>

namespace sygaldry { namespace components { namespace esp32 {

enum class ButtonActive {High, Low};

template<gpio_num_t pin_number, ButtonActive active_level = ButtonActive::Low>
struct Button
: name_<"Button">
, author_<"Travis J. West">
, copyright_<"Travis J. West (C) 2023">
, description_<"A single button attached to a GPIO">
, ButtonGestureModel
{
    using gpio = GPIO<pin_number>;

    void init()
    {
        gpio::init();
        gpio::input_mode();
        if constexpr (active_level == ButtonActive::Low) gpio::enable_pullup();
        else gpio::enable_pulldown();
    }

    void operator()()
    {
        inputs.button_state = (char)gpio::level();
        ButtonGestureModel::operator()();
    }
};
    
} } }
