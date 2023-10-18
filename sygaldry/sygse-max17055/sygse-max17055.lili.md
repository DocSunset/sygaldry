\page page-sygse-max17055 MAX17055 on ESP32

Like the Trill Craft only a simple component is needed for the MAX17055 to work in ESP32 builds.

```cmake
# @#'CMakeLists.txt'
set(lib sygse-max17055)
add_library(${lib} STATIC)
target_include_directories(${lib} PUBLIC .)
target_sources(${lib} PRIVATE ${lib}.cpp)
target_link_libraries(${lib}
        PUBLIC sygse-arduino_hack
        PUBLIC sygsa-max17055
        )

target_link_libraries(sygse INTERFACE ${lib})
# @/
```

```cpp
// @#'sygse-max17055.hpp'
#pragma once
#include "sygsa-max17055.hpp"
// @/

// @#'sygse-max17055.cpp'
#include "sygsa-max17055.impl.hpp"
// @/
```
