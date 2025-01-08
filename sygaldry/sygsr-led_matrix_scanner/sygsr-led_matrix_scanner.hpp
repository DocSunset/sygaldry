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

namespace sygaldry { namespace sygsr {

/// \addtogroup sygsr
/// \{

/// \defgroup sygsr-led_matrix_scanner sygsr-led_matrix_scanner: LED Matrix Scanner
/// Literate source code: page-sygsr-led_matrix_scanner
/// \{

/*! \brief Drives a bank of output pins from an array. Template args give array source and GPIO pin mapping.
*/
template<typename coordinate_t, std::size_t ROWS, std::size_t COLUMNS, unsigned int row_pins[ROWS], unsigned int col_pins[COLUMNS]>
struct LedMatrixScanner
: name_<"LED Matrix Scanner">
, description_<"Drives a matrix of LEDs one at a time">
, author_<"Travis J. West">
, copyright_<"Copyright 2024 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    unsigned int last_out[2] = {0,0};

    void init_pin(unsigned int pin)
    {
        gpio_init(pin);
        gpio_set_dir(pin, true); // output
        gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_12MA);
    }

    void init()
    {
        for (std::size_t row = 0; row < ROWS;    ++row) init_pin(row_pins[row]);
        for (std::size_t col = 0; col < COLUMNS; ++col) init_pin(col_pins[col]);
    }

    void main(const coordinate_t& coord)
    {
        if (coord[0] != last_out[0])
        {
            // columns are connected to anodes
            // and are normally low (no source of current),
            // active high (gpio sources current)
            gpio_put(col_pins[last_out[0]], 0);
            gpio_put(col_pins[coord[0]], 1);
            last_out[0] = coord[0];
        }
        if (coord[1] != last_out[1])
        {
            // rows are connected to cathodes through a resistor
            // and are normally high (no sink for current),
            // active low (gpio sinks current)
            gpio_put(row_pins[last_out[1]], 1);
            gpio_put(row_pins[coord[1]], 0);
            last_out[1] = coord[1];
        }
    }
};

/// \}
/// \}

} }
