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
#include <sygah-metadata.hpp>
#include <sygah-endpoints.hpp>

namespace sygaldry { namespace sygse {
///\addtogroup sygse
///\{
///\defgroup sygse-gpio sygse-gpio: ESP32 GPIO
/// Literate source code: \ref page-sygse-gpio
///\{

template<gpio_num_t pin_number>
struct GPIO
: name_<"GPIO Pin">
, author_<"Travis J. West">
, copyright_<"Travis J. West (C) 2023">
, description_<"An ESP-IDF GPIO API wrapper as a message-based `sygaldry` component">
{
    #define gpio_function(c_name, esp_idf_func, ... )\
    static auto c_name() noexcept\
    {\
        auto ret = esp_idf_func(__VA_ARGS__);\
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);\
        return ret;\
    }
    // all following input methods return esp_err_t, either ESP_OK or an error code
    gpio_function(remove_interrupt_handler, gpio_isr_handler_remove, pin_number); // "remove interrupt handler", "remove the interrupt handler callback for this pin",
    static auto interrupt_handler(void (*handler)(void*), void* args) noexcept
    {
        auto ret = gpio_isr_handler_add(pin_number, handler, args);
        ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
        return ret;
    }
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
    static auto level() noexcept {
        return gpio_get_level(pin_number);
    }

    static auto drive_capability() noexcept {
        auto ret = GPIO_DRIVE_CAP_DEFAULT;
        gpio_get_drive_capability(pin_number, &ret);
        return ret;
    }
    #undef gpio_function
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
};

///\}
///\}
} }
