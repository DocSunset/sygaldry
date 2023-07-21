# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: LGPL-2.1-or-later

# https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBLO REQUIRED liblo)
add_executable(liblo-tests tests.cpp)
target_link_libraries(liblo-tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(liblo-tests PRIVATE ${LIBLO_LIBRARIES})
target_include_directories(liblo-tests PRIVATE ${LIBLO_INCLUDE_DIRS})
catch_discover_tests(liblo-tests)
