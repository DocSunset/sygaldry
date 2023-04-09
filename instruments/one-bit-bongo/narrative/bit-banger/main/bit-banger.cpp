#include "driver/gpio.h"

constexpr gpio_num_t input_pin  = GPIO_NUM_4;
constexpr gpio_num_t output_pin = GPIO_NUM_18;

extern "C" void app_main(void)
{
    // https://docs.espressif.com/projects/esp-idf/en/v5.0.1/esp32/api-reference/peripherals/gpio.html#_CPPv413gpio_config_t
    gpio_config_t input_cfg =
    {
        .pin_bit_mask = 1ULL<<input_pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config_t output_cfg =
    {
        .pin_bit_mask = 1ULL<<output_pin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    gpio_config(&input_cfg);
    gpio_config(&output_cfg);
    for(;;)
    {
        gpio_set_level(output_pin, 1^gpio_get_level(input_pin));
    }
}
