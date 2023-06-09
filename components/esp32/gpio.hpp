#pragma once

#include <driver/gpio.h>
#include <hal/gpio_types.h>
#include <helpers/metadata.hpp>
#include <helpers/endpoints.hpp>

namespace sygaldry { namespace components { namespace esp32 {

using namespace sygaldry::helpers;

template<gpio_num_t pin_number>
struct GPIO
    : name_<"ESP32 GPIO Pin">
    , author_<"Travis J. West">
    , copyright_<"Travis J. West (C) 2023">
    , description_<"An ESP-IDF GPIO API wrapper as a message-based `sygaldry` component">
{
    #define gpio_function(c_name, name, description, esp_idf_func, ... )\
    [[no_unique_address]] struct c_name##_t : name_<name>, description_<description> {\
        auto operator()(GPIO<pin_number>& gpio) const noexcept {\
            if (gpio.inputs.bypass != 0) return ESP_FAIL;\
            auto ret = esp_idf_func(__VA_ARGS__);\
            ESP_ERROR_CHECK_WITHOUT_ABORT(ret);\
            if (ret != ESP_OK) gpio.inputs.bypass = true;\
            return ret;\
        }\
    } c_name /* remember the semicolon */
    struct inputs_t {
        // when bypass is not zero, all other endpoints are disabled and return ESP_FAIL
        toggle<"bypass"> bypass;

        // all following input methods return esp_err_t, either ESP_OK or an error code
        gpio_function(remove_interrupt_handler, "remove interrupt handler", "remove the interrupt handler callback for this pin", gpio_isr_handler_remove, pin_number);
        [[no_unique_address]] struct interrupt_handler_t : name_<"interrupt handler">
                , description_<"set the interrupt handler callback for this pin"> {
            auto operator()(GPIO<pin_number>& gpio, void (*handler)(void*), void* args) const noexcept {
                if (gpio.inputs.bypass != 0) return ESP_FAIL;
                auto ret = gpio_isr_handler_add(pin_number, handler, args);
                ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
                if (ret != ESP_OK) gpio.inputs.bypass = 1;
                return ret;
            }
        } interrupt_handler;
        [[no_unique_address]] struct uninstall_isr_service_t
        : name_<"uninstall ISR service">
        , description_<"uninstall the IDF-provided ISR service and free its resources"> 
        {
            auto operator()(GPIO<pin_number>& gpio, int intr_alloc_flags) const noexcept {
                if (gpio.inputs.bypass != 0) return;
                gpio_uninstall_isr_service();
            }
        } uninstall_isr_service;

        // the installer must only be called once for all GPIO
        [[no_unique_address]] struct install_isr_service_t
        : name_<"install ISR service">
        , description_<"install the IDF-provided ISR service, enabling per-pin interrupts">
        {
            auto operator()(GPIO<pin_number>& gpio, int intr_alloc_flags) const noexcept {
                if (gpio.inputs.bypass != 0) return ESP_FAIL;
                auto ret = gpio_install_isr_service(intr_alloc_flags);
                ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
                if (ret != ESP_OK) gpio.inputs.bypass = true;
                return ret;
            }
        } install_isr_service;

        // The documentation warns "ESP32: Please do not use the interrupt of GPIO36 and GPIO39 when using ADC or Wi-Fi and Bluetooth with sleep mode enabled."
        gpio_function(enable_interrupt,  "enable interrupt",  "enable interrupt",  gpio_intr_enable, pin_number);
        gpio_function(disable_interrupt, "disable interrupt", "disable interrupt", gpio_intr_disable, pin_number);

        gpio_function(reset,        "reset",        "reset pin and disconnect peripheral outputs", gpio_reset_pin,     pin_number);
        gpio_function(rising_edge,  "rising edge",  "interrupt on rising edge",                    gpio_set_intr_type, pin_number, GPIO_INTR_POSEDGE);
        gpio_function(falling_edge, "falling edge", "interrupt on falling edge",                   gpio_set_intr_type, pin_number, GPIO_INTR_NEGEDGE);
        gpio_function(any_edge,     "any edge",     "interrupt on any edge",                       gpio_set_intr_type, pin_number, GPIO_INTR_ANYEDGE);
        gpio_function(low_level,    "low level",    "interrupt on low level trigger",              gpio_set_intr_type, pin_number, GPIO_INTR_LOW_LEVEL);
        gpio_function(high_level,   "high level",   "interrupt on high level trigger",             gpio_set_intr_type, pin_number, GPIO_INTR_HIGH_LEVEL);
        gpio_function(high,         "high",         "set output level high",                       gpio_set_level,     pin_number, 1);
        gpio_function(low,          "low",          "set output level low",                        gpio_set_level,     pin_number, 0);

        gpio_function(disable_pin, "disable pin", "disable input and output", gpio_set_direction, pin_number, GPIO_MODE_DISABLE);
        gpio_function(input_mode, "input mode", "set pin mode to input", gpio_set_direction, pin_number, GPIO_MODE_INPUT);
        gpio_function(output_mode, "output mode", "set pin mode to output", gpio_set_direction, pin_number, GPIO_MODE_OUTPUT);
        gpio_function(output_od_mode, "output od mode", "set pin mode to output with open-drain", gpio_set_direction, pin_number, GPIO_MODE_OUTPUT_OD);
        gpio_function(input_output_mode, "input output mode", "set pin mode to input/output", gpio_set_direction, pin_number, GPIO_MODE_INPUT_OUTPUT);
        gpio_function(input_output_od_mode, "input output od mode", "set pin mode to input/output with open-drain", gpio_set_direction, pin_number, GPIO_MODE_INPUT_OUTPUT_OD);

        // note: pins 34-39 have no pull resistors
        gpio_function(enable_pullup,               "enable pull-up",                 "enable internal pull-up resistor",                      gpio_set_pull_mode,        pin_number, GPIO_PULLUP_ONLY);
        gpio_function(enable_pulldown,             "enable pull-down",               "enable internal pull-down resistor",                    gpio_set_pull_mode,        pin_number, GPIO_PULLDOWN_ONLY);
        gpio_function(enable_pullup_and_pulldown,  "enable pull-up and pull-down",   "enable both internal pull-up and pull-down resistors",  gpio_set_pull_mode,        pin_number, GPIO_PULLUP_PULLDOWN);
        gpio_function(disable_pullup_and_pulldown, "disable pull-up and pull-down",  "disable both internal pull-up and pull-down resistors", gpio_set_pull_mode,        pin_number, GPIO_FLOATING);
        gpio_function(disable_pullup,              "disable pull-up",                "disable internal pull-up resistor",                     gpio_pullup_dis,           pin_number);
        gpio_function(disable_pulldown,            "disable pull-down",              "disable internal pull-down resistor",                   gpio_pulldown_dis,         pin_number);

        gpio_function(rising_edge_wakeup,  "rising edge wake-up",  "enable wake-up on rising edge",        gpio_wakeup_enable,  pin_number, GPIO_INTR_POSEDGE);
        gpio_function(falling_edge_wakeup, "falling edge wake-up", "enable wake-up on falling edge",       gpio_wakeup_enable,  pin_number, GPIO_INTR_NEGEDGE);
        gpio_function(any_edge_wakeup,     "any edge wake-up",     "enable wake-up on any edge",           gpio_wakeup_enable,  pin_number, GPIO_INTR_ANYEDGE);
        gpio_function(low_level_wakeup,    "low level wake-up",    "enable wake-up on low level trigger",  gpio_wakeup_enable,  pin_number, GPIO_INTR_LOW_LEVEL);
        gpio_function(high_level_wakeup,   "high level wake-up",   "enable wake-up on high level trigger", gpio_wakeup_enable,  pin_number, GPIO_INTR_HIGH_LEVEL);
        gpio_function(disable_wakeup,      "disable wake-up",      "disable wake-up functionality",        gpio_wakeup_disable, pin_number);
        gpio_function(wakeup_high,         "wake-up high",         "enable wake-up on high level",         gpio_wakeup_enable, pin_number, GPIO_INTR_HIGH_LEVEL);
        gpio_function(wakeup_low,          "enable wake-up",       "enable wake-up on low level",          gpio_wakeup_enable, pin_number, GPIO_INTR_LOW_LEVEL);

        gpio_function(set_drive_weakest,   "set drive weakest",   "set drive capability", gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_0);
        gpio_function(set_drive_weak,      "set drive weak",      "set drive capability", gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_1);
        gpio_function(set_drive_medium,    "set drive medium",    "set drive capability", gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_2);
        gpio_function(set_drive_strong,    "set drive strong",    "set drive capability", gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_DEFAULT);
        gpio_function(set_drive_strongest, "set drive strongest", "set drive capability", gpio_set_drive_capability, pin_number, GPIO_DRIVE_CAP_3);

        // TODO: hold, sleep hold, deep sleep hold, iomux, sleep sel, sleep dir, sleep pull, rtc pins, ...
    } inputs;

    [[no_unique_address]] struct outputs_t {
        [[no_unique_address]] struct level_t
        : name_<"level">
        , description_<"GPIO input level (always zero for output pin)">
        {
            auto operator()() const noexcept {
                return gpio_get_level(pin_number);
            }
        } level;

        [[no_unique_address]] struct drive_capability_t
        : name_<"drive capability">
        , description_<"drive strength capability">
        {
            auto operator()() const noexcept {
                auto ret = GPIO_DRIVE_CAP_DEFAULT;
                gpio_get_drive_capability(pin_number, &ret);
                return ret;
            }
        } drive_capability;
    } outputs;

    #undef gpio_function

    void init()
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

        inputs.reset(*this);
    }
};

} } }
