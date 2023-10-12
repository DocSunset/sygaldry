\page page-sygXX-new_component sygXX-new_component: New Component

Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

Introductory prose goes here.

Remember to add your new component to the top level CMakeLists.txt!

I repeat: Remember to add your new component to the top level CMakeLists.txt!

```cpp
// @#'sygXX-newcomponent.hpp'
#pragma once
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

// include statements

namespace sygaldry { namespace sygXX {

/// \addtogroup syXX
/// \{

/// \defgroup sygXX-newcomponent sygXX-newcomponent: New Component
/// Literate source code: page-sygXX-newcomponent
/// \{

/*! \brief brief doc

detailed doc
*/
struct NewComponent
: name_<"New Component">
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

} }
// @/
```

```cpp
// @#'sygXX-newcomponent.cpp'
/*
Copyright 2023 Travis J. West, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/
#include "sygXX-newcomponent.hpp"

namespace sygaldry { namespace sygXX {

void NewComponent::init()
{
}

void NewComponent::main()
{
}

} }
// @/
```

```cpp
// @#'sygXX-newcomponent.test.cpp'
#include <catch2/catch_test_macros.hpp>
#include "sygXX-newcomponent.hpp"

using namespace sygaldry;
using namespace sygaldry::sygXX;

@{tests}
// @/
```

```cmake
# @#'CMakeLists.txt'
set(lib sygXX-newcomponent)
add_library(${lib} STATIC)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_include_directories(${lib} PUBLIC .)
target_link_libraries(${lib}
        PUBLIC sygah-endpoints
        PUBLIC sygaldry-helpers-metadata
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
