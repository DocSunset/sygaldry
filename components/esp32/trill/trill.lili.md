\page comp_trill_esp32 Trill on ESP32

Only a very simple IDF component is required to make the `Trill-Arduino` library
available for ESP32 builds.

```cmake
# @#'CMakeLists.txt'
set(LIB sygaldry-components-esp32-trill)
add_library(${LIB} STATIC)
target_include_directories(${LIB} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(${LIB} PRIVATE "${SYGALDRY_ROOT}/dependencies/Trill-Arduino")
target_sources(${LIB} PRIVATE ${LIB}.cpp ${SYGALDRY_ROOT}/dependencies/Trill-Arduino/Trill.cpp)
target_link_libraries(${LIB} PUBLIC sygaldry-components-esp32-arduino_hack)
target_link_libraries(${LIB} PUBLIC sygaldry-components-trill_craft)
# @/
```

```cpp
// @#'sygaldry-components-esp32-trill.cpp'
#include "sygaldry-components-trill_craft.impl.hpp"
// @/
```
