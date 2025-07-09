#pragma once
/*
Copyright 2024 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"
#include "sygse-gpio.hpp"
#include "sygup-cstdio_logger.hpp"

namespace sygaldry { namespace sygse {

/// \addtogroup sygse
/// \{

/// \defgroup sygse-pin-driver-array sygse-pin-driver-array: Pin Driver Array
/// Literate source code: page-sygse-pin-driver-array
/// \{

/*! \brief Drives a bank of output pins from an array. Template args give array source and GPIO pin mapping.
*/
template<typename array_t, gpio_num_t ... pins>
struct PinDriverArray
: name_<"Pin Driver Array">
, description_<"Drives a bank of output pins from an array">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    sygup::CstdioLogger log;
    void init()
    {
        (GPIO<pins>::output_mode(), ...);
    }

    template<gpio_num_t pin>
    void set_pin(bool a)
    {
        if (a) GPIO<pin>::high();
        else GPIO<pin>::low();
    }

    template<std::size_t... i>
    void set_pins(const array_t& a, std::index_sequence<i...>)
    {
        (set_pin<pins>(a[i]), ...);
    }

    void main(const array_t& a)
    {
        static_assert(sizeof...(pins) == a.size());
        set_pins(a, std::make_index_sequence<sizeof...(pins)>{});
    }
};

/// \}
/// \}

} }
