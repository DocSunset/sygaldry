\page page-syghe-pins syghe-pins: ESP-IDF Pin Number Hardware Abstraction Layer

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: SPDX-License-Identifier: MIT

[TOC]

Pin numbers hardware abstraction layer.

When building a project with ESP-IDF, the specific ESP32 microcontroller is
termed the "target" of the build (not to be confused with e.g. release vs debug
build targets or compilation targets as seen in e.g.
`target_include_directories` and other related CMake functions). The IDF makes
a property `IDF_TARGET` available to CMake via `idf_build_get_property(varname
PROPNAME)`. We use this information to configure a header file that is then
made available by this software component.

```cpp
// @#'syghe-pins.hpp.in'
#pragma once
/*
Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music 
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music 
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ. 
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT
*/

#cmakedefine SYGALDRY_ESP32
#cmakedefine SYGALDRY_ESP32S3
#cmakedefine SYGALDRY_I2C_PINS

namespace sygaldry { namespace syghe {

/// \addtogroup syghe syghe: ESP-IDF HAL
/// \{

/// \defgroup syghe-pins syghe-pins: ESP-IDF Pin Number Hardware Abstraction Layer
/// Literate source code: page-syghe-pins
/// \{

/*! \defgroup syghe-pins-i2c syghe-pins: I2C Pins
*/
/// \{
#if defined SYGALDRY_I2C_PINS
constexpr int I2C_MAIN_SDA = ${SYGALDRY_SDA};
constexpr int I2C_MAIN_SCL = ${SYGALDRY_SCL};
#elif defined SYGALDRY_ESP32
constexpr int I2C_MAIN_SDA = 21;
constexpr int I2C_MAIN_SCL = 22;
#elif defined SYGALDRY_ESP32S3
constexpr int I2C_MAIN_SDA = 8;
constexpr int I2C_MAIN_SCL = 9;
#endif
/// \}

/*! \defgroup sygse-adc-channels sygse-adc: ADC Channels

The correspondance between ADC channels and GPIO numbers can be verified by
referring to the Technical Reference Manual, pages 60 and 634, tables 4-3 and
29-2 for ESP32, and pages 311 table 2-7 and page 1425 table 39-6 for ESP32s3
Not all channels are necessarily available on all development boards, and some
channels may have other functions that conflict with their use for ADC
conversions. Beware!
*/
/// \{
enum AdcChannel
{
#if defined SYGALDRY_ESP32
ADC1_CHANNEL_4 = 32, ///< ADC1 Channel 4 (aka GPIO32, 32K_XP)
ADC1_CHANNEL_5 = 33, ///< ADC1 Channel 5 (aka GPIO33, 32K_XN)
ADC1_CHANNEL_6 = 34, ///< ADC1 Channel 6 (aka GPIO34, VDET_1)
ADC1_CHANNEL_7 = 35, ///< ADC1 Channel 7 (aka GPIO35, VDET_2)
ADC1_CHANNEL_0 = 36, ///< ADC1 Channel 0 (aka GPIO36, SENSOR_VP)
ADC1_CHANNEL_1 = 37, ///< ADC1 Channel 1 (aka GPIO37, SENSOR_CAPP)
ADC1_CHANNEL_2 = 38, ///< ADC1 Channel 2 (aka GPIO38, SENSOR_CAPN)
ADC1_CHANNEL_3 = 39 ///< ADC1 Channel 3 (aka GPIO39, SENSOR_VN)
#elif defined SYGALDRY_ESP32S3
ADC1_CHANNEL_0 = 1 , ///< aka GPIO1 
ADC1_CHANNEL_1 = 2 , ///< aka GPIO2 
ADC1_CHANNEL_2 = 3 , ///< aka GPIO3 
ADC1_CHANNEL_3 = 4 , ///< aka GPIO4 
ADC1_CHANNEL_4 = 5 , ///< aka GPIO5 
ADC1_CHANNEL_5 = 6 , ///< aka GPIO6 
ADC1_CHANNEL_6 = 7 , ///< aka GPIO7 
ADC1_CHANNEL_7 = 8 , ///< aka GPIO8
ADC1_CHANNEL_8 = 9 , ///< aka GPIO9 
ADC1_CHANNEL_9 = 10, ///< aka GPIO10
ADC2_CHANNEL_0 = 11, ///< aka GPIO11
ADC2_CHANNEL_1 = 12, ///< aka GPIO12
ADC2_CHANNEL_2 = 13, ///< aka GPIO13
ADC2_CHANNEL_3 = 14, ///< aka GPIO14
ADC2_CHANNEL_4 = 15, ///< aka GPIO15
ADC2_CHANNEL_5 = 16, ///< aka GPIO16
ADC2_CHANNEL_6 = 17, ///< aka GPIO17
ADC2_CHANNEL_7 = 18, ///< aka GPIO18
ADC2_CHANNEL_8 = 19, ///< aka GPIO19
ADC2_CHANNEL_9 = 20 ///< aka GPIO20
#endif
};

/// \}


/// \}
/// \}

} }
// @/
```

```cmake
# @#'CMakeLists.txt'
idf_build_get_property(target IDF_TARGET)
if(${target} MATCHES esp32s3)
    set(SYGALDRY_ESP32S3 1)
    if(SYGALDRY_I2C_PINS)
        message("Using SYGALDRY_SDA and SYGALDRY_SCL")
     endif()
elseif(${target} MATCHES esp32)
    set(SYGALDRY_ESP32 1)
else()
    message(FATAL_ERROR "Unrecognized IDF_TARGET value ${target}; syghe-pins may require an update?")
endif()

set(lib syghe-pins)
configure_file(${lib}.hpp.in ${lib}.hpp)
add_library(${lib} INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/${lib}.hpp")
target_include_directories(${lib} INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
# @/
```
