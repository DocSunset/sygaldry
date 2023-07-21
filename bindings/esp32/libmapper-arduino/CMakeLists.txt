# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: LGPL-2.1-or-later

idf_component_register(SRC_DIRS "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/compat"
                                "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/lo"
                                "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/mapper"
                                "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src/zlib"
                       INCLUDE_DIRS "${SYGALDRY_ROOT}/dependencies/libmapper-arduino/src"
                       PRIV_REQUIRES esp_netif)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DHAVE_CONFIG_H" "-Wno-error=char-subscripts" "-Wno-error=format-truncation" "-Wno-error=format")
