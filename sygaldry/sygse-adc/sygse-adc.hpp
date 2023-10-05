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

namespace sygaldry { namespace sygse {

/*! \addtogroup sygse ESP32 Sensors */
/// \{
/*! \defgroup sygse-adc Oneshot ADC Driver */
///\{

/*! \defgroup sygse-adc-channels ADC Channels

The correspondance between ADC channels and GPIO numbers can be verified by
referring to [the ESP32 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf),
pages 60 and 634, tables 4-3 and 29-2. Not all channels are necessarily available
on all development boards, and some channels may have other functions that
conflict with their use for ADC conversions. Beware!
*/
/// \{
static constexpr int ADC1_CHANNEL_0 = 36; ///< ADC1 Channel 0 (aka GPIO36, SENSOR_VP)
static constexpr int ADC1_CHANNEL_1 = 37; ///< ADC1 Channel 1 (aka GPIO37, SENSOR_CAPP)
static constexpr int ADC1_CHANNEL_2 = 38; ///< ADC1 Channel 2 (aka GPIO38, SENSOR_CAPN)
static constexpr int ADC1_CHANNEL_3 = 39; ///< ADC1 Channel 3 (aka GPIO39, SENSOR_VN)
static constexpr int ADC1_CHANNEL_4 = 32; ///< ADC1 Channel 4 (aka GPIO32, 32K_XP)
static constexpr int ADC1_CHANNEL_5 = 33; ///< ADC1 Channel 5 (aka GPIO33, 32K_XN)
static constexpr int ADC1_CHANNEL_6 = 34; ///< ADC1 Channel 6 (aka GPIO34, VDET_1)
static constexpr int ADC1_CHANNEL_7 = 35; ///< ADC1 Channel 7 (aka GPIO35, VDET_2)
/// \}

/*! \brief Driver component for the analog-digital converter.

*OneshotAdc* configures analog-digital converter 1 (ADC1) to read in oneshot
mode from the given GPIO pin and polls its current value in each loop.

\warning The allowable pins are given explicit template instantiation
declarations. Using a different pin will result in a link-time error.

\tparam gpio_num The GPIO number associated with the channel to read.
                 See \ref sygse-adc-intantiations for
                 guidance on which GPIO number corresponds to which ADC
                 channel.
*/
template<int gpio_num>
struct OneshotAdc
: name_<"ESP32 Oneshot ADC">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
{
    static_assert(ADC1_CHANNEL_4 <= gpio_num && gpio_num <= ADC1_CHANNEL_3);

    struct outputs_t {
        slider<"raw", "raw binary representation of the analog voltage measured by the ADC"
        , int, 0, 4096, 0 // esp32 technical reference manual V5 section 29.3 says 12 bit max resolution
        > raw;
    } outputs;

    void * pimpl;

    /// Configure the ADC1 channel for the given gpio_num in oneshot read mode.
    void init();

    /// Poll the current value of the given ADC1 channel.
    void main();
};

// it is presumed that the linker will discard instantiations that are not used,
// so that the code size of the final executable is not influenced by the extra
// instantiations here.
extern template struct OneshotAdc<ADC1_CHANNEL_0>;
extern template struct OneshotAdc<ADC1_CHANNEL_1>;
extern template struct OneshotAdc<ADC1_CHANNEL_2>;
extern template struct OneshotAdc<ADC1_CHANNEL_3>;
extern template struct OneshotAdc<ADC1_CHANNEL_4>;
extern template struct OneshotAdc<ADC1_CHANNEL_5>;
extern template struct OneshotAdc<ADC1_CHANNEL_6>;
extern template struct OneshotAdc<ADC1_CHANNEL_7>;

/// \}
/// \}

} }
