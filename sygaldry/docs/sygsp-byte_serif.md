\page page-sygsp-byte_serif sygsp-byte_serif: Portable Byte-wise Serial Interface

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: SPDX-License-Identifier: MIT

[TOC]

The byte-wise serial interface represents a common idiom when interacting with
sensor integrated circuits, where the sensor is interacted with via its control
registers--essentially a bank of memory-mapped switches, usually accessed over
either I2C or SPI. Most interactions will be to read the data output registers;
besides this, almost all interactions will be to read and/or write a single
byte to/from a single register.

In line with this operational structure, we wish to define an interface that is
independent of the particular serial bus used (whether I2C or SPI), as well as
the specificies of that bus (such as the I2C address or SPI CS pin), and any
platform-specific concerns of implementation. We then anticipate that sensor
components, such as [the ICM20948](\ref page-sygsp-icm20948) will accept a
realization of this interface as a template parameter, and use it to access
the hardware. In other words, we are using compile-time polymorphism via
templates.

The byte serif is expected to define methods `read` and `write` for single byte
interaction with a particular register at a given address, and an overload of
`read` taking an `uint8_t` array and size for reading multiple sequential bytes.

Currently the only implementation of this conceptual interface is the
Arduino `TwoWire`-based I2C serial interface made for use with the ICM20948
motion sensor component.

```cpp
// @#'sygsp-byte_serif.hpp'
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

namespace sygaldry { namespace sygsp {

/// \addtogroup sygsp
/// \{

/// \defgroup sygsp-byte_serif sygsp-byte_serif: Portable Byte-wise Serial Interface
/// Literate source code: page-sygsp-byte_serif
/// \{

/*! \brief Byte-wise serial interface for sensors
*/
struct ByteSerif
: name_<"Byte Serif">
, description_<"Byte-wise serial interface for sensors">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct inputs_t {
    } inputs;

    struct outputs_t {
    } outputs;

    void init();

    void main();
};

/// \}
/// \}

} }
// @/
```

```cpp
// @#'sygsp-byte_serif.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsp-byte_serif.hpp"

namespace sygaldry { namespace sygsp {

void ByteSerif::init()
{
}

void ByteSerif::main()
{
}

} }
// @/
```

```cpp
// @#'sygsp-byte_serif.test.cpp'
#include <catch2/catch_test_macros.hpp>
#include "sygsp-byte_serif.hpp"

using namespace sygaldry;
using namespace sygaldry::sygsp;

// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsp-byte_serif)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_include_directories(${lib} PUBLIC .)
target_link_libraries(${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygah-metadata
        )

if (SYGALDRY_BUILD_TESTS)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${lib}-test PRIVATE ${lib})
#target_link_libraries(${lib}-test PRIVATE OTHERREQUIREDPACKAGESANDCOMPONENTSHERE)
catch_discover_tests(${lib}-test)
endif()
# @/
```
