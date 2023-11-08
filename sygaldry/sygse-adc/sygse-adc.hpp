#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-endpoints.hpp"
#include "sygah-metadata.hpp"
#include "syghe-pins.hpp"

namespace sygaldry { namespace sygse {

/*! \addtogroup sygse sygse: ESP32 Sensors */
/// \{
/*! \defgroup sygse-adc sygse-adc: Oneshot ADC Driver */
///\{

/// Output endpoints for OneshotAdc
struct OneshotAdcOutputs {
    slider<"raw", "raw binary representation of the analog voltage measured by the ADC"
    , int, 0, 4096, 0 // esp32 technical reference manual V5 section 29.3 says 12 bit max resolution
    > raw;
};

namespace detail {
struct OneshotAdcImpl
{
    using outputs_t = OneshotAdcOutputs;
    void * pimpl;
    void init(int gpio_num, outputs_t& outputs);
    void main(outputs_t& outputs);
};
}

/*! \brief Driver component for the analog-digital converter.

*OneshotAdc* configures analog-digital converter 1 (ADC1) to read in oneshot
mode from the given GPIO pin and polls its current value in each loop.

\tparam gpio_num The GPIO number associated with the channel to read.
                 See \ref sygse-adc-intantiations for
                 guidance on which GPIO number corresponds to which ADC
                 channel.
*/
template<syghe::AdcChannel gpio_num>
struct OneshotAdc
: name_<"ESP32 Oneshot ADC">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
{
    using outputs_t = OneshotAdcOutputs;
    outputs_t outputs;
    detail::OneshotAdcImpl pimpl;

    /// Configure the ADC1 channel for the given gpio_num in oneshot read mode.
    void init() {pimpl.init(gpio_num, outputs);}

    /// Poll the current value of the given ADC1 channel.
    void main() {pimpl.main(outputs);}
};

/// \}
/// \}

} }
