\page page-docs-writing_a_new_component Writing a New Component

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

Work in progress!

Remember to add your new component to the top level CMakeLists.txt!

\subpage page-sygXY-new_sensor New Sensor

# New Component Template

\page page-sygXY-new_sensor New Sensor

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Literate explication of the design and implementation goes here.

```cpp
// @#'sygaldry-sensors-esp32-newsensor.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

// include statements

namespace sygaldry { namespace sensors { namespace esp32 {

/*! \addtogroup sygaldry-sensors-esp32 ESP32 Sensors
*/
/// \{

/*! \defgroup sygaldry-sensors-esp32-newsensor New Sensor
*/
/// \{

/*! \brief brief doc

detailed doc
*/
struct NewSensor
: name_<"New Sensor">
, description_<"Description goes here">
, author_<"Travis J. West">
, copyright_<"Copyright 2023 Sygaldry Contributors">
, license_<"SPDX-License-Identifier: MIT">
, version_<"0.0.0">
{
    struct inputs_t {
    } inputs;

    struct outputs_t {
    } outputs;

    /// brief doc
    void init();

    /// brief doc
    void main();
}

/// \}
/// \}

} } }
// @/
```

```cpp
// @#'sygaldry-sensors-esp32-newsensor.cpp'
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygaldry-sensors-esp32-newsensor"

namespace sygaldry { namespace sensors { namespace esp32 {

void NewSensor::init()
{
}

void NewSensor::main()
{
}

} } }
// @/
```

```cpp
// @#'sygaldry-sensors-esp32-newsensor.test.cpp'
#include "sygaldry-sensors-esp32-newsensor.hpp"

// test suite setup

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygaldry-sensors-esp32-newsensor)
add_library(${lib} STATIC)
    target_sources(${lib}
            PRIVATE ${lib}.cpp
            )
    target_include_directories(${lib}
            PUBLIC .
            )
    target_link_libraries(${lib}
            PUBLIC sygah-endpoints
            PUBLIC sygaldry-helpers-metadata
            )
target_link_libraries(sygaldry-sensors-esp32 INTERFACE ${lib})

# build automation for the test suite goes here

# @/
```
