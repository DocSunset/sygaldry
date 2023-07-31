\page bind_rapidjson RapidJSON ESP-IDF Component

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
(IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
(CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

The below CMakeLists.txt file declares an ESP-IDF component by simply
identifying the relevant include directory for RapidJSON.

```cmake
# @#'rapidjson/CMakeLists.txt'
idf_component_register(INCLUDE_DIRS "${SYGALDRY_ROOT}/dependencies/rapidjson/include")
# @/
```
