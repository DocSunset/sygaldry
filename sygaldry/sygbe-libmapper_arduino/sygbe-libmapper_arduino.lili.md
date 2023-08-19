\page page-sygbe-libmapper_arduino Libmapper Arduino Library

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: LGPL-2.1-or-later

The `libmapper-arduino` library provides libmapper and liblo bindings for the
ESP32. This library was initially developed by
[Mathias Bredholt](https://github.com/mathiasbredholt) of
[Torso Electronics](https://torsoelectronics.com/) during
[his MA thesis research](https://escholarship.mcgill.ca/concern/theses/gx41mp578)
at [IDMIL](https://www.idmil.org/).

The library provides a handful of compatibility fixes to enable liblo and
libmapper to run on top of the ESP-IDF's POSIX-like framework, as well as
consolidating all of the header and source files for these libraries to enable
them to be built statically without having to involve the GNU build and library
packaging tools (M4, Perl, Autoconf, Automake, Libtool, etc.) that these
libraries use by default.

The below CMakeLists.txt file in turn declares an ESP-IDF component by simply
identifying the relevant source files. It's also necessary to add some compiler
flags, since the default warning/error settings of ESP-IDF trigger compilation
failure on several warnings that are considered tolerable by the liblo and
libmapper developers. It remains as future work to open a pull request to
integrate this IDF component `CMakeLists.txt` file in the upstream library.

```cmake
# @#'CMakeLists.txt'
# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: LGPL-2.1-or-later

set(lib sygbe-libmapper_arduino)
add_library(${lib} STATIC)
target_sources(${lib}
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/compat/gai_strerror.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/compat/gethostname.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/compat/getnameinfo.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/compat/ifaddrs.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/address.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/blob.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/bundle.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/message.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/method.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/pattern_match.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/send.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/server.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/server_thread.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/timetag.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo/version.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/device.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/expression.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/graph.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/link.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/list.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/map.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/network.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/object.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/properties.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/router.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/signal.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/slot.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/table.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/time.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper/value.c"
        PRIVATE "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/zlib/crc32.c"
        )
target_include_directories(${lib}
        PUBLIC "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src"
        )
target_link_libraries(${lib} PRIVATE idf::esp_netif)
target_compile_options(${lib} PRIVATE "-DHAVE_CONFIG_H" "-Wno-error=char-subscripts" "-Wno-error=format-truncation" "-Wno-error=format")
# @/
```
