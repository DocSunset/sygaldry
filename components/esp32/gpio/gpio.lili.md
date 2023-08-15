\page comp_gpio_esp32 ESP32 GPIO

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document implements type safe and lightly error-checked wrapper that
attempts to expose all available behavior for a single GPIO pin through static
methods of a template class, for use in ESP32 `sygaldry` components. It is
currently implemented via the ESP-IDF. Not all functionality has been wrapped
yet, and much of it remains un-tested. GPIO reference documentation for the
current version of the ESP-IDF is found
[here](https://docs.espressif.com/projects/esp-idf/en/v5.0.2/esp32/api-reference/peripherals/gpio.html).
The relevant public domain example code is found
[here](https://github.com/espressif/esp-idf/blob/v5.0.2/examples/peripherals/gpio/generic_gpio/main/gpio_example_main.c).

The ESP-IDF provides a very uniform interface for interacting with GPIO. Almost
all methods return an `esp_err_t` error code, and all arguments (with the
exception of a few related to interrupts) are either pin numbers or
enumerations; it can reasonably be assumed that these values are known at
compile time in almost all cases.

Our strategy for wrapping this API is to declare static methods without
arguments. Each method calls one API function, checks the error code, and
returns it. Because all of these method implementations are identical except
for the names of things, we use a macro to facilitate implementation without
repeating ourselves.

# Endpoints

```cpp
// @='gpio_function macro'
#define gpio_function(c_name, esp_idf_func, ... )\
static auto c_name() noexcept\
{\
    auto ret = esp_idf_func(__VA_ARGS__);\
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);\
    return ret;\
}
// @/
```

Most (TODO: all) of the subroutines in the ESP-IDF GPIO API are then wrapped
in this manner.

# Component Wishes

It would be convenient to encapsulate this functionality in
a true component, treating the functions as message inputs, so that a textual
name and description could be provided alongside, so that the component serves
as an executable manual. However, there are issues with creating reflectable
message endpoints that are also convenient to use in a subcomponent. For
instance, we could declare the endpoint as a functor. However, since the
functor has no way to access the data of the parent component, the parent
must be passed in as an argument:

```cpp
struct my_message_t : name_<"my message">, description_<"a useful hint"> {
    void operator()(Parent& parent) {
        /* ... */
    }
} my_message;

/* ... */

my_component.inputs.my_message(my_component);
```

Such an implementation also generally compiles such that the message endpoint
take up space, despite that it has no state. The approach taken in Avendish
is for message endpoints to return a function; this nicely solves the latter
issue while making the interface for a user of the message endpoint in
a subcomponent quite unfriendly, e.g. for a message with no arguments:

```cpp
decltype(my_component)::messages::my_message::func()(my_component);
```

For now, we draw the line at messages. Any endpoint that can be represented
with value semantics, we allow, and any component that would require messages,
we avoid. Hence, our GPIO wrapper cannot yet be implemented as a true component.
However, name strings and descriptions left over from an attempt to implement
such a component are retained as comments, in hopes they may be useful one day
beyond being documentation...

## Inputs

```cpp
// @='input wrappers'
// all following input methods return esp_err_t, either ESP_OK or an error code
gpio_function(remove_interrupt_handler, gpio_isr_handler_remove, pin_number); // "remove interrupt handler", "remove the interrupt handler callback for this pin",
@{weird inputs}

// The documentation warns "ESP32: Please do not use the interrupt of GPIO36 and GPIO39 when using ADC or Wi-Fi and Bluetooth with sleep mode enabled."
gpio_function(enable_interrupt,  gpio_intr_enable,  pin_number); // "enable interrupt",  "enable interrupt",
gpio_function(disable_interrupt, gpio_intr_disable, pin_number); // "disable interrupt", "disable interrupt",

gpio_function(reset,        gpio_reset_pin,     pin_number);                       // "reset",        "reset pin and disconnect peripheral outputs",
gpio_function(rising_edge,  gpio_set_intr_type, pin_number, GPIO_INTR_POSEDGE);    // "rising edge",  "interrupt on rising edge",
gpio_function(falling_edge, gpio_set_intr_type, pin_number, GPIO_INTR_NEGEDGE);    // "falling edge", "interrupt on falling edge",
gpio_function(any_edge,     gpio_set_intr_type, pin_number, GPIO_INTR_ANYEDGE);    // "any edge",     "interrupt on any edge",
gpio_function(low_level,    gpio_set_intr_type, pin_number, GPIO_INTR_LOW_LEVEL);  // "low level",    "interrupt on low level trigger",
gpio_function(high_level,   gpio_set_intr_type, pin_number, GPIO_INTR_HIGH_LEVEL); // "high level",   "interrupt on high level trigger",
gpio_function(high,         gpio_set_level,     pin_number, 1);                    // "high",         "set output level high",
gpio_function(low,          gpio_set_level,     pin_number, 0);                    // "low",          "set output level low",

gpio_function(disable_pin,          gpio_set_direction, pin_number, GPIO_MODE_DISABLE);         // "disable pin", "disable input and output",
gpio_function(input_mode,           gpio_set_direction, pin_number, GPIO_MODE_INPUT);           // "input mode", "set pin mode to input",
gpio_function(output_mode,          gpio_set_direction, pin_number, GPIO_MODE_OUTPUT);          // "output mode", "set pin mode to output",
gpio_function(output_od_mode,       gpio_set_direction, pin_number, GPIO_MODE_OUTPUT_OD);       // "output od mode", "set pin mode to output with open-drain",
gpio_function(input_output_mode,    gpio_set_direction, pin_number, GPIO_MODE_INPUT_OUTPUT);    // "input output mode", "set pin mode to input/output",
gpio_function(input_output_od_mode, gpio_set_direction, pin_number, GPIO_MODE_INPUT_OUTPUT_OD); // "input output od mode", "set pin mode to input/output with open-drain",

// note: pins 34-39 have no pull resistors
gpio_function(enable_pullup,               gpio_set_pull_mode,        pin_number, GPIO_PULLUP_ONLY);     // "enable pull-up",                "enable internal pull-up resistor",
gpio_function(enable_pulldown,             gpio_set_pull_mode,        pin_number, GPIO_PULLDOWN_ONLY);   // "enable pull-down",              "enable internal pull-down resistor",
gpio_function(enable_pullup_and_pulldown,  gpio_set_pull_mode,        pin_number, GPIO_PULLUP_PULLDOWN); // "enable pull-up and pull-down",  "enable both internal pull-up and pull-down resistors",
gpio_function(disable_pullup_and_pulldown, gpio_set_pull_mode,        pin_number, GPIO_FLOATING);        // "disable pull-up and pull-down", "disable both internal pull-up and pull-down resistors",
gpio_function(disable_pullup,              gpio_pullup_dis,           pin_number);                       // "disable pull-up",               "disable internal pull-up resistor",
gpio_function(disable_pulldown,            gpio_pulldown_dis,         pin_number);                       // "disable pull-down",             "disable internal pull-down resistor",

gpio_function(rising_edge_wakeup,  gpio_wakeup_enable,  pin_number, GPIO_INTR_POSEDGE);    // "rising edge wake-up",  "enable wake-up on rising edge",
gpio_function(falling_edge_wakeup, gpio_wakeup_enable,  pin_number, GPIO_INTR_NEGEDGE);    // "falling edge wake-up", "enable wake-up on falling edge",
gpio_function(any_edge_wakeup,     gpio_wakeup_enable,  pin_number, GPIO_INTR_ANYEDGE);    // "any edge wake-up",     "enable wake-up on any edge",
gpio_function(low_level_wakeup,    gpio_wakeup_enable,  pin_number, GPIO_INTR_LOW_LEVEL);  // "low level wake-up",    "enable wake-up on low level trigger",
gpio_function(high_level_wakeup,   gpio_wakeup_enable,  pin_number, GPIO_INTR_HIGH_LEVEL); // "high level wake-up",   "enable wake-up on high level trigger",
gpio_function(disable_wakeup,      gpio_wakeup_disable, pin_number);                       // "disable wake-up",      "disable wake-up functionality",
gpio_function(wakeup_high,         gpio_wakeup_enable,  pin_number, GPIO_INTR_HIGH_LEVEL); // "wake-up high",         "enable wake-up on high level",
gpio_function(wakeup_low,          gpio_wakeup_enable,  pin_number, GPIO_INTR_LOW_LEVEL);  // "enable wake-up",       "enable wake-up on low level",

gpio_function(set_drive_weakest,   gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_0);       // "set drive weakest",   "set drive capability",
gpio_function(set_drive_weak,      gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_1);       // "set drive weak",      "set drive capability",
gpio_function(set_drive_medium,    gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_2);       // "set drive medium",    "set drive capability",
gpio_function(set_drive_strong,    gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_DEFAULT); // "set drive strong",    "set drive capability",
gpio_function(set_drive_strongest, gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_3);       // "set drive strongest", "set drive capability",

// TODO: hold, sleep hold, deep sleep hold, iomux, sleep sel, sleep dir, sleep pull, rtc pins, ...
// @/
```

### Interrupt Handler

A few API calls require unusual arguments or have different return values.
These are implemented seperately, incurring a small but hopefully tolerable
amount of duplication.

As previously mentioned, the input port to install an interrupt handler is
an exception to the general pattern. The component defers design of an ISR
to the user, so this port accept a pointer to the ISR function and its
context as arguments and passes them to the ESP-IDF method.

```cpp
// @+'weird inputs'
static auto interrupt_handler(void (*handler)(void*), void* args) noexcept
{
    auto ret = gpio_isr_handler_add(pin_number, handler, args);
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    return ret;
}
// @/
```

Similarly, reponsibility for the interrupt allocation flags for the
IDF-provided ISR service is also deferred to the user. The ISR uninstaller has
no return value, so it also requires a unique implementation.

```cpp
// @+'weird inputs'
static void uninstall_isr_service() noexcept
{
    gpio_uninstall_isr_service();
}

// the installer must only be called once for all GPIO
static auto install_isr_service(int intr_alloc_flags) noexcept
{
    auto ret = gpio_install_isr_service(intr_alloc_flags);
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    return ret;
}
// @/
```

## Outputs

There are significantly fewer output endpoints, since reading data from the
GPIO is considerably less involved than configuring it just right.

One thing to note: since the only possible error for gpio_get_drive_capability
is ESP_ERR_INVALID_ARG and the only arg that could be invalid is the pin number
could be invalid, or the pointer could be null since we can statically
guarantee that neither of these is the case, we can ignore the error code from
this IDF function and avoid having to return the drive_capability by output
argument from out port, and instead implement it as a getter. Similarly,
`gpio_get_level` never returns an error, so the output endpoint for this API
can also be implemented as a getter.

```cpp
// @='output wrappers'
static auto level() noexcept {
    return gpio_get_level(pin_number);
}

static auto drive_capability() noexcept {
    auto ret = GPIO_DRIVE_CAP_DEFAULT;
    gpio_get_drive_capability(pin_number, &ret);
    return ret;
}
// @/
```

# Initialization and Pin Number Assertions

The GPIO doesn't actually require much initialization. A call to
`inputs.reset()` is more than adequate. We take the opportunity presented by
the method, however, to assert certain requirements on the pin number. Although
the ESP32 has up to 39 pins, many of these cannot conventionally be used for
one reason or another as GPIO. Pins 0 to 3 (pins 0 and 1 for strapping and pins
2 and 3 for UART) are used for programming and pins 6 to 11, 16, and 17 are
used for SPI flash memory--these pins cannot be used as GPIO in almost any
application. Furthermore: pins 12 to 15 are used for debugging with JTAG; pin
12 strapping additionally sets the LDO voltage regulator's output voltage at
boot; pins 5 and 15 strapping additionally set SDIO timing and debug logging
behaviors at boot; pins 20 and 28 to 31 are not mentioned in the documentation,
nor the datasheet, suggesting that these hypothetical GPIO do not exist; pins
18, 19, 21, 22, and 23 are also used for the `VSPI` serial peripheral
interface; pins 25 to 27 cannot be used at the same time as WiFi; and pins 32
to 39 are shared with one of the analog-to-digital converters. Indeed, there is
not a single pin on the ESP32 that is not multi-purpose. It is a GPIO starved
platform.

The most detailed documentation on pin functions can be found in the datasheet.
The documentation also provides additional guidance. The pinout diagram for
a given MCU board can offer further advice where available.

```cpp
// @='init function with assertions'
static void init()
{
    static_assert(GPIO_NUM_0 <= pin_number && pin_number <= GPIO_NUM_39,
        "pin number invalid");

    // comment these out if you really know what you're doing!
    static_assert(pin_number != GPIO_NUM_0, "GPIO0 is an important strapping pin"
            "used during boot to determine SPI boot (pulled up, default) or"
            "download boot (pulled down). It should not be used for GPIO");
    static_assert(pin_number != GPIO_NUM_1, "GPIO1 is UART TXD, used for"
            "programming, and should not be used for GPIO");
    static_assert(pin_number != GPIO_NUM_2, "GPIO2 is an important strapping pin"
            "that must be pulled down during boot to initiate firmware download."
            "It should not be used for GPIO");
    static_assert(pin_number != GPIO_NUM_3, "GPIO3 is UART_RXD, used for"
            "programming, and should not be used for GPIO");
    static_assert(!(GPIO_NUM_6 <= pin_number && pin_number <= GPIO_NUM_11)
                && pin_number != GPIO_NUM_16 && pin_number != GPIO_NUM_17,
            "GPIO6-11, 16, and 17 are used by SPI flash memory and shoult not be"
            "used for GPIO");
    static_assert(pin_number != GPIO_NUM_20 && !(GPIO_NUM_28 <= pin_number && pin_number <= GPIO_NUM_32),
            "GPIO20, and 28-32 likely don't exist, and can't be used for GPIO");

    reset();
}
// @/
```

# Summary

```cpp
// @#'sygaldry-components-esp32-gpio.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <driver/gpio.h>
#include <hal/gpio_types.h>
#include <sygaldry-helpers-metadata.hpp>
#include <sygaldry-helpers-endpoints.hpp>

namespace sygaldry { namespace components { namespace esp32 {

template<gpio_num_t pin_number>
struct GPIO
: name_<"GPIO Pin">
, author_<"Travis J. West">
, copyright_<"Travis J. West (C) 2023">
, description_<"An ESP-IDF GPIO API wrapper as a message-based `sygaldry` component">
{
    @{gpio_function macro}
    @{input wrappers};
    @{output wrappers};
    #undef gpio_function
    @{init function with assertions}
};

} } }
// @/
```

# Tests

At the time of writing, we test only the bare minimum functionality required to
read a single button in a polling loop.

```cpp
// @#'sygaldry-components-esp32-gpio.test.cpp'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <sygaldry-components-esp32-gpio.hpp>

void gpio()
{
    using pin = sygaldry::components::esp32::GPIO<GPIO_NUM_23>;
    pin::init();
    pin::input_mode();
    pin::enable_pullup();
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, pin::level(), "input mode pin with pullup should read high level");
    pin::disable_pullup();
    pin::enable_pulldown();
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pin::level(), "input mode pin with pulldown should read low level");
}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygaldry-components-esp32-gpio)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
# @/
```
