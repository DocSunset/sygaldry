# Trill Sygaldry Component

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

Currently only one wrapper is provided for Trill Craft which simply exposes
the sensor's 30 touch readings.

[TOC]

# Trill Craft

```cpp
// @#'trill_craft.hpp'
#pragma once

// platform specific methods are required to make this header directly available
#include "Trill.h"
#include "helpers/metadata.hpp"

namespace sygaldry { namespace components {

struct TrillCraft
: name_<"Trill Craft">
{
};

} }
// @/
```
