/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsr-adc.hpp"
#include <hardware/adc.h>

namespace sygaldry { namespace sygsr {

template<unsigned int input_num>
void OneshotAdc<input_num>::init()
{
    adc_init();
    adc_gpio_init(ADC_GPIO[input_num]);
}

template<unsigned int input_num>
void OneshotAdc<input_num>::main()
{
    adc_select_input(input_num);
    outputs.raw = (int)adc_read();
}

template struct OneshotAdc<ADC_CHANNEL_0>;
template struct OneshotAdc<ADC_CHANNEL_1>;
template struct OneshotAdc<ADC_CHANNEL_2>;
template struct OneshotAdc<ADC_CHANNEL_3>;

} }
