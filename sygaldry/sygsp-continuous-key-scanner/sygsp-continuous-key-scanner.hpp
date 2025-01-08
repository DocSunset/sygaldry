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
#include <cmath>

namespace sygaldry { namespace sygsp {

/// \addtogroup sygsp
/// \{

/// \defgroup sygsp-continuous-key-scanner sygsp-continuous-key-scanner: Continuous Key Scanner
/// Literate source code: page-sygsp-continuous-key-scanner
/// \{

/*! \brief Scan a bank of near-field infrared sensors connected to an associated ADC and GPIO pins

\tparam N The number of keys to scan. The output arrays have size N.
\tparam adc_reading_t Type of the ADC raw data output endpoint to be passed to the main subroutine.
*/
template<typename adc_reading_t, int adc_max, std::size_t ROWS, std::size_t COLUMNS>
struct KeyScanner
: name_<"Continuous Key Scanner">
, description_<"Scan a matrix of near-field infrared sensors connected to an associated ADC and GPIO pins">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct inputs_t {
    } inputs;

    struct outputs_t {
        array_message<"keys", ROWS*COLUMNS, "sensor output state", float> keys;
        array_message<"velocity", ROWS*COLUMNS, "rate of key movement", float> velo;
        array_message<"raw", ROWS*COLUMNS, "raw sensor output state", int> raw;
        array_message<"last raw", ROWS*COLUMNS, "previous raw sensor output state", int> lastraw;
        /// Connect this to an appropriate component to drive the LEDs of the infrared sensors
        array_message<"LED coordinate", 2, "coordinate of the lit LED in the matrix", char> leds;
    } outputs;

    std::size_t current_key{};

    void set_led() { outputs.leds = {static_cast<char>(current_key % COLUMNS), static_cast<char>(current_key / COLUMNS)}; }

    void init()
    {
        current_key = 0;
        set_led();
    }

    void main(const adc_reading_t& adc_reading)
    {
        outputs.raw[current_key] = adc_reading;
        if (++current_key >= ROWS*COLUMNS)
        {
            current_key = 0;
            for (std::size_t i = 0; i < ROWS*COLUMNS; ++i)
            {
                // impromptu 1euro-ish filter implementation
                constexpr float dt = 1/20.0f; // this needs to be measured rather than set
                constexpr float dalpha = 0.1;
                constexpr float beta = 2.5f;
                const float delta = outputs.raw[i] - outputs.lastraw[i];
                outputs.velo[i] = dalpha * outputs.raw[i] + (1.0f-dalpha) * outputs.velo[i];
                const float cut = 0.1f + beta * std::abs(outputs.velo[i] / 4096.0f);
                const float tau = 1.0f/(6.2831853f*cut);
                const float te = dt;
                const float xalpha = 1.0f / (1.0f + tau/te);
                outputs.keys[i] = xalpha * outputs.raw[i] + (1.0f-xalpha) * outputs.keys[i];
            }
            outputs.raw.set_updated();
            outputs.keys.set_updated();
            outputs.velo.set_updated();
        }
        set_led();
    }
};

/// \}
/// \}

} }
