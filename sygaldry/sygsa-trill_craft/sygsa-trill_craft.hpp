#pragma once
/*
Copyright 2021-2023 Edu Meneses https://www.edumeneses.com, Metalab - Société des
Arts Technologiques (SAT), Input Devices and Music Interaction Laboratory
(IDMIL), McGill University

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#include <limits>
#include "sygah-metadata.hpp"
#include "sygah-endpoints.hpp"

namespace sygaldry { namespace components { namespace arduino {

struct TrillCraft
: name_<"Trill Craft">
, description_<"a capacitive touch sensor with 30 electrodes to be connected by the user">
, author_<"Edu Meneses, Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    static constexpr unsigned int channels = 30;
    struct inputs_t {
        // TODO: set baseline, speed and resolution, noise threshold, prescaler
        //array<"map", channels, etc.> map;
    } inputs;

    struct outputs_t {
        toggle<"running", "indicates successful connection and polling status"> running;
        text_message<"error status", "indicates error conditions"> error_status;

        array<"raw", channels
             , "unprocessed difference in capacitance with respect to baseline"
             , int, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0
             // TOOD: what are the actual minimum and maximum values that can be found?
             > raw;
        toggle<"any", "indicates presence of any touch"> any;
        slider<"instant max", "the maximum reading currently"
              , int, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0
              > instant_max;
        slider<"max seen", "the maximum reading seen since boot"
              , int, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0
              // arguably this should be session data
              > max_seen;
        array<"mask", channels, "indicates which electrodes are touched"
             , char, 0, 1, 0
             > mask;
        array<"normalized", channels
             , "normalized capacitance reading"
             > normalized;
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

    /// Attempt to read raw data, producing updated instant maximum, normlized, and discretized values
    void main();
};

} } }
