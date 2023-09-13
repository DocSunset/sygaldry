\page page-sygsa-trill_craft Arduino Trill Craft

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

[Trill](https://bela.io/products/trill/) is a capacitive touch sensing platform
by Bela. Several different form factors are available. This document describes
the Sygaldry wrapper around the `Trill-Arduino` library. `Trill-Arduino` has only
a logical dependency on the Arduino API. As such, `Trill-Arduino` and the following
Trill Sygaldry components are physically hardware independent, although they do
require the availability of `Arduino.h` or `WProgram.h` and `Wire.h`.
The [Arduino hack subsystem](components/arduino-hack.lili.md) provides these
headers in cases where a first party Arduino library is not available, with
platform specific implementations in the relevant subdirectories. See the
Arduino hack subsystem documentation for more information.

Currently only one wrapper is provided for Trill Craft, which simply exposes
the sensor's 30 touch readings. This is largely adapted from Edu Meneses'
implementation for a previous version of the T-Stick firmware.

# Trill Craft

```cpp
// @#'sygsa-trill_craft.hpp'
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

namespace sygaldry { namespace components { namespace arduino {

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
                      , uint8_t, 0, 3, 1
                      , tag_session_data
                      > speed;
        slider_message<"resolution"
                      , "measurement resolution in bits; "
                        "higher resolutions reduce effective sampling rate"
                      , uint8_t, 1, 8, 1
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

} } }
// @/

// @#'sygsa-trill_craft.impl.hpp'
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

#include "sygsa-trill_craft.hpp"
#include <algorithm>
#include <Trill.h>

namespace sygaldry { namespace components { namespace arduino {

void TrillCraft::init()
{
    auto trill = new Trill();
    pimpl = static_cast<void*>(trill);

    // TODO: initialize *all* input parameters in case there is no session data
    bool initialize_map = false;
    std::array<bool, channels> channel_indexed{0};
    for (std::size_t i = 0; i < channels && !initialize_map; ++i)
    {
        if (inputs.map[i] < 30)
        {
            if (channel_indexed[inputs.map[i]]) initialize_map = true; // channel already indexed
            else channel_indexed[inputs.map[i]] = true;
        }
        else
        {
            initialize_map = true;
        }
    }
    if (initialize_map) for (std::size_t i = 0; i < channels; ++i) inputs.map[i] = i;

    int setup_return_code = trill->setup(Trill::TRILL_CRAFT);
    if (0 != setup_return_code)
    {
        outputs.running = false;

        // See Trill.cpp for an authoritative source of information
        // on the return codes, which are not otherwise documented
        switch (setup_return_code)
        {
        case -2:
            outputs.error_status = "unknown default address";
            break;
        case 2:
            outputs.error_status = "unable to identify device";
            break;
        case -3:
            outputs.error_status = "found unexpected device type";
            break;
        case -1:
            outputs.error_status = "invalid device mode";
            break;
        default:
            outputs.error_status = "unknown error";
        }
        return;
    }
    else outputs.running = true;
}

void TrillCraft::main()
{
    if (not outputs.running) return; // TODO: try to reconnect every so often

    auto trill = static_cast<Trill*>(pimpl);

    // TODO: we should check and constrain boundary conditions
    if (inputs.speed.updated || inputs.resolution.updated) trill->setScanSettings(inputs.speed, inputs.resolution);
    if (inputs.noise_threshold.updated)                    trill->setNoiseThreshold(inputs.noise_threshold);
    //if (inputs.autoscan_interval.updated)                trill->setAutoScanInterval(inputs.autoscan_interval);
    if (inputs.prescaler.updated)                          trill->setPrescaler(inputs.prescaler);
    if (inputs.resolution.updated || inputs.prescaler.updated || inputs.update_baseline)
    {
        for (auto& max : outputs.max_seen.value) max = 0;
        trill->updateBaseline();
    }

    trill->requestRawData();
    for (int i=0; i<30; i++) {
        if (trill->rawDataAvailable() > 0) {
            outputs.raw[i] = trill->rawDataRead();
        }
    }

    for (int i = 0; i < channels; i++) {
        if (outputs.raw[i] != 0) {
            outputs.max_seen[i] = std::max(outputs.max_seen[i], outputs.raw[i]);
            outputs.normalized[inputs.map[i]] = static_cast<float>(outputs.raw[i])
                                              / static_cast<float>(outputs.max_seen[i]);
            outputs.mask[inputs.map[i]] = true;
        } else {
            outputs.normalized[inputs.map[i]] = 0.0f;
            outputs.mask[inputs.map[i]] = false;
        }
    }

    outputs.instant_max = *std::max_element( outputs.normalized.value.data()
                                           , outputs.normalized.value.data()+channels
                                           );

    if (outputs.instant_max == 0.0f) {
        outputs.any = 0;
    } else {
        outputs.any = 1;
    }
}

} } }
// @/
```

TODO: write tests. And documentation...

```cmake
# @#'CMakeLists.txt'
set(lib sygsa-trill_craft)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygah)
target_link_libraries(sygsa INTERFACE ${lib})
# @/
```
