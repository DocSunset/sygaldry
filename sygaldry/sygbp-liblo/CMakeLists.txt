# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: LGPL-2.1-or-later

set(lib sygbp-liblo)
add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE .)
target_link_libraries(${lib}
        INTERFACE sygac
        INTERFACE sygah
        INTERFACE sygbp-osc_string_constants
        )


if (SYGALDRY_BUILD_TESTS)
# https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBLO REQUIRED liblo)
add_executable(${lib}-test ${lib}.test.cpp)
target_link_libraries(${lib}-test
        PRIVATE Catch2::Catch2WithMain
        PRIVATE ${LIBLO_LIBRARIES}
        PRIVATE ${lib}
        PRIVATE sygac-components
        PRIVATE sygbp-test_component
        )
target_include_directories(${lib}-test PRIVATE ${LIBLO_INCLUDE_DIRS})
catch_discover_tests(${lib}-test)
endif()
