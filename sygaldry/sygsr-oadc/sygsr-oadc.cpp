/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsr-oadc.hpp"
#include <hardware/adc.h>

namespace sygaldry { namespace sygsr {

template<unsigned int input_num>
void OversamplingAdc<input_num>::init()
{
    adc_init();
    adc_gpio_init(OADC_GPIO[input_num]);
}

template<unsigned int input_num>
void OversamplingAdc<input_num>::main()
{
    adc_select_input(input_num);
    outputs.raw = 0;
    for (unsigned int n = 1; n < oversampling_factor+1; ++n)
    {
        if (n <= skip_factor) {adc_read(); continue;}
        outputs.raw += ((float)adc_read() - outputs.raw) / (n-skip_factor);
    }
}

template struct OversamplingAdc<OADC_CHANNEL_0>;
template struct OversamplingAdc<OADC_CHANNEL_1>;
template struct OversamplingAdc<OADC_CHANNEL_2>;
template struct OversamplingAdc<ADC_CHANNEL_3>;

} }
