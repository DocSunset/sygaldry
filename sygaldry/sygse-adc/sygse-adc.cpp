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
