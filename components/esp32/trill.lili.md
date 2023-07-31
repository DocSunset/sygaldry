\page comp_trill_esp32 Trill on ESP32

Only a very simple IDF component is required to make the `Trill-Arduino` library
available for ESP32 builds.

```cmake
# @#'trill/CMakeLists.txt'
idf_component_register( SRCS ${SYGALDRY_ROOT}/dependencies/Trill-Arduino/Trill.cpp
                        INCLUDE_DIRS ${SYGALDRY_ROOT}/dependencies/Trill-Arduino
                        REQUIRES arduino-hack
                      )
# @/
```
