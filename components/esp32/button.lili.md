\page comp_button_esp32 ESP32 Button

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

This component is essentially simple, and a nice demonstration of
how at some point components start to come together almost magically
to make very high level constructions. All of the functionality of
this component is basically implemented elsewhere. The `GPIO` component
handles interaction with the hardware, and the `ButtonGestureModel`
component applies the required interpretation to the information from
the hardware, returning a rich set of higher-level descriptors of the
button's state and its evolution over time. All this implementation has
left to do is set up the GPIO for the purpose, and forward its data to
the gesture model.

```cpp
// @#'button.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <components/sensors/button.hpp>
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
// @/
```

# Tests

For now we just check if it compiles. It's assumed that since most of the
button's functionality is implemented in other (presumably well tested)
components, that the trivial implementation above can be verified by
inspection. If the button implementation here ever grows more complex, e.g.
incorporating interrupts or what have you, then tests should be incorporated at
that time as well.

```cpp
// @#'tests/button.hpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <components/esp32/button.hpp>

void button()
{
    sygaldry::components::esp32::Button<GPIO_NUM_23> button; 
    button.init();
    button();
}
// @/
```
