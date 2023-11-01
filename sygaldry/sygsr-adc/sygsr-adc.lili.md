\page page-sygsr-adc sygsr-adc: Raspberry Pi Pico SDK ADC Driver

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: SPDX-License-Identifier: MIT

[TOC]

Oneshot analog-digital converter

```cpp
// @#'sygsr-adc.hpp'
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

namespace sygaldry { namespace sygsr {

/// \addtogroup sygsr
/// \{

/// \defgroup sygsr-adc sygsr-adc: Raspberry Pi Pico SDK ADC Driver
/// Literate source code: page-sygsr-adc
/// \{

/*! \brief Oneshot analog-digital converter
*/
struct ADC
: name_<"ADC">
, description_<"Oneshot analog-digital converter">
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
// @#'sygsr-adc.cpp'
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygsr-adc.hpp"

namespace sygaldry { namespace sygsr {

void ADC::init()
{
}

void ADC::main()
{
}

} }
// @/
```

```cpp
// @#'sygsr-adc.test.cpp'
#include <catch2/catch_test_macros.hpp>
#include "sygsr-adc.hpp"

using namespace sygaldry;
using namespace sygaldry::sygsr;

// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygsr-adc)
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
