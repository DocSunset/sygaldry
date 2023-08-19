\page page-sygse-trill Trill on ESP32

Only a very simple IDF component is required to make the `Trill-Arduino` library
available for ESP32 builds.

```cmake
# @#'CMakeLists.txt'
set(lib sygse-trill)
add_library(${lib} STATIC)
target_include_directories(${lib}
        PUBLIC .
        PRIVATE ${SYGALDRY_ROOT}/dependencies/Trill-Arduino
        )
target_sources(${lib}
        PRIVATE ${lib}.cpp
        PRIVATE ${SYGALDRY_ROOT}/dependencies/Trill-Arduino/Trill.cpp
        )
target_link_libraries(${lib}
        PUBLIC sygse-arduino_hack
        PUBLIC sygsa-trill_craft
        )

target_link_libraries(sygse INTERFACE ${lib})
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
