\page comp_trill Trill

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

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
implementation for the T-Stick firmware.

# Trill Craft

```cpp
// @#'sygaldry-components-trill_craft.hpp'
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
#include "sygaldry-helpers-metadata.hpp"
#include "sygaldry-helpers-endpoints.hpp"

namespace sygaldry { namespace components {

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

} }
// @/

// @#'sygaldry-components-trill_craft.impl.hpp'
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

#include "sygaldry-components-trill_craft.hpp"
#include <algorithm>
#include <Trill.h>

namespace sygaldry { namespace components {

void TrillCraft::init()
{
    auto trill = new Trill();
    pimpl = static_cast<void*>(trill);
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
    trill->requestRawData();
    for (int i=0; i<30; i++) {
        if (trill->rawDataAvailable() > 0) {
            outputs.raw[i] = trill->rawDataRead();
        }
    }

    outputs.instant_max = *std::max_element( outputs.raw.value.data()
                                           , outputs.raw.value.data()+channels
                                           );

    if (outputs.instant_max == 0) {
        outputs.any = 0;
    } else {
        outputs.any = 1;
    }

    outputs.max_seen = std::max(outputs.max_seen.value, outputs.instant_max.value);

    for (int i = 0; i < channels; i++) {
        if (outputs.raw[i] != 0) {
            outputs.mask[i] = true;
            outputs.normalized[i] = static_cast<float>(outputs.raw[i])
                                  / static_cast<float>(outputs.max_seen);
        } else {
            outputs.mask[i] = false;
        }
    }
}

} }
// @/
```

TODO: write tests. And documentation...

```cmake
# @#'CMakeLists.txt'
set(lib sygaldry-components-trill_craft)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib} INTERFACE sygaldry-helpers)
# @/
```
