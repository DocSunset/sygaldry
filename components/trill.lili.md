# Trill Sygaldry Component

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
// @#'trill_craft.hpp'
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

// platform specific methods are required to make this header directly available
#include "Trill.h"
#include "helpers/metadata.hpp"

namespace sygaldry { namespace components {

struct TrillCraft
: name_<"Trill Craft">
, description_<"A capacitive touch sensor with 30 electrodes to be connected by the user.">
, author_<"Edu Meneses, Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
};

} }
// @/
```
