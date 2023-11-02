#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace sygsr {

/// \addtogroup sygsr
/// \{

/// \defgroup sygsr-adc sygsr-adc: Raspberry Pi Pico SDK ADC Driver
/// Literate source code: page-sygsr-adc
/// \{

/*! \defgroup sygsr-adc-channels sygsr-adc: ADC Channels

The internal temperature sensors is currently not supported through this
component.
*/
/// \{
static constexpr unsigned int ADC_CHANNEL_0 = 0;
static constexpr unsigned int ADC_CHANNEL_1 = 1;
static constexpr unsigned int ADC_CHANNEL_2 = 2;
static constexpr unsigned int ADC_CHANNEL_3 = 3;
static constexpr unsigned int ADC_GPIO[] = {26,27,28,29};
/// \}

/*! \brief Oneshot analog-digital converter
*/
template<unsigned int input_number>
struct OneshotAdc
: name_<"ADC">
, description_<"Oneshot analog-digital converter">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    static_assert(ADC_CHANNEL_0 <= input_number && input_number <= ADC_CHANNEL_3);

    struct outputs_t {
        slider<"raw", "raw binary representation of the analog voltage measured by the ADC"
        , int, 0, 4096, 0
        > raw;
    } outputs;

    void init();

    void main();
};

extern template struct OneshotAdc<ADC_CHANNEL_0>;
extern template struct OneshotAdc<ADC_CHANNEL_1>;
extern template struct OneshotAdc<ADC_CHANNEL_2>;
extern template struct OneshotAdc<ADC_CHANNEL_3>;

/// \}
/// \}

} }
