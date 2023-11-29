#pragma once
/*
Copyright 2021-2023 Edu Meneses https://www.edumeneses.com, Metalab - Société
des Arts Technologiques (SAT), Input Devices and Music Interaction Laboratory
(IDMIL), McGill University

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <limits>
#include <cstdint>
#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace sygsa {
///\addtogroup sygsa
///\{
///\defgroup sygsa-trill_craft sygsa-trill_craft: Arduino Trill Craft
///\{

struct TrillCraft
: name_<"Trill Craft">
, description_<"A capacitive touch sensor with 30 electrodes to be connected by "
               "the user. Operates in DIFF mode only. See "
               "https://learn.bela.io/using-trill/settings-and-sensitivity/ "
               "for more information on configuration."
              >
, author_<"Edu Meneses, Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    static constexpr unsigned int channels = 30;
    struct inputs_t {
        // TODO: set baseline, speed and resolution, noise threshold, prescaler
        slider_message<"prescaler", "measurement gain adjustment"
                      , uint8_t, 1, 8, 1
                      , tag_session_data
                      > prescaler;
        slider_message<"noise threshold", "threshold for ignoring low-level noise"
                      , uint8_t, 0, 255, 0
                      , tag_session_data
                      > noise_threshold;
        slider_message<"sampling speed"
                      , "0 - ultra fast (57 to 2800 us per sensor), "
                        "3 - slow (205 to 22000 us per sensor). "
                        "Broad sampling rate adjustment; see also resolution"
                      , uint8_t, 0, 3, 0
                      , tag_session_data
                      > speed;
        slider_message<"resolution"
                      , "measurement resolution in bits; "
                        "higher resolutions reduce effective sampling rate"
                      , uint8_t, 9, 16, 9
                      , tag_session_data
                      > resolution;
        bng<"update baseline"
           , "reset the baseline capacitance based on the current sensor state."
             "Avoid touching the sensor while updating the baseline reading."
           > update_baseline;

        /* NOTE: the IDAC value method provided by the Trill API seems not to be of
        any use. According to the datasheet linked from the Bela API documentation

        (https://www.infineon.com/dgdl/Infineon-CapSense_Sigma-Delta_Datasheet_CSD-Software+Module+Datasheets-v02_02-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f9f5d9b0e82&redirId=File_1_2_579),

        this value is overridden by the firmware by default. So don't worry about
        adding an endpoint for setIDACValue()! */

        // TODO: autoscan, event pin interrupt

        array<"map", channels
             , "mapping from raw channel to normalized channel, "
               "e.g. if the 0th element in the map is 5, then the 0th raw"
               "channel will be written to the normalized channel with index 5."
             , int, 0, 29, 0
             , tag_session_data
             > map;
    } inputs;

    struct outputs_t {
        toggle<"running", "indicates successful connection and polling status"> running;
        text_message<"error status", "indicates error conditions"> error_status;

        array<"raw", channels
             , "unprocessed difference in capacitance with respect to baseline" // Technically not the actual raw measurement, but close enough?
             , int, 0, std::numeric_limits<int>::max(), 0
             // TOOD: what are the actual minimum and maximum values that can be found?
             > raw;
        toggle<"any", "indicates presence of any touch"> any;
        array<"max seen", channels, "the maximum reading seen"
              , int, 0, decltype(raw)::max(), 0
              > max_seen;
        array<"normalized", channels
             , "normalized capacitance reading"
             > normalized;
        array<"mask", channels, "indicates which electrodes are touched"
             , char, 0, 1, 0
             > mask;
        slider<"instant max", "the maximum normalized reading currently"
              , float
              > instant_max;
    } outputs;

    void * pimpl = nullptr;

    /*! \brief Try to connect to the Trill sensor at the default address.

    \detail As well as connecting to the sensor and checking for
    connection errors, this also sets the initial baseline capacitance
    values and initializes the measurement rate and resolution.

    \warning `Trill::setup()`, called by this function, calls `Wire.begin()`;
    if the I2C bus requires particular set up, it should be done before this
    component is initialized.

    */
    void init();

    /// Update configuration parameters and attempt to read raw data, producing updated instant maximum, normalized, and discretized values
    void main();
};

///\}
///\}
} }
