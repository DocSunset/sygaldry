\page page-sygse-trill sygse-trill: Trill on ESP32

Only a very simple component is required to make the `Trill-Arduino` library
available for ESP32 builds.

```cmake
# @#'CMakeLists.txt'
set(lib sygse-trill)
add_library(${lib} INTERFACE)
target_include_directories(${lib}
        INTERFACE .
        INTERFACE ${SYGALDRY_ROOT}/dependencies/Trill-Arduino
        )
target_sources(${lib}
        INTERFACE ${lib}.cpp
        INTERFACE ${SYGALDRY_ROOT}/dependencies/Trill-Arduino/Trill.cpp
        )
target_link_libraries(${lib}
        INTERFACE sygse-arduino_hack
        INTERFACE sygsa-trill_craft
        )
# @/
```

```cpp
// @#'sygse-trill.hpp'
#pragma once
#include "sygsa-trill_craft.hpp"
// @/

// @#'sygse-trill.cpp'
#include "sygsa-trill_craft.impl.hpp"
// @/
```
