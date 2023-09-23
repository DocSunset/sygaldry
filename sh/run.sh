#!/bin/sh -e

# create the build directory if necessary, then build and test the project

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

./sh/lili.sh || exit 1
[ "$#" -gt 0 ] && dir="$1" || dir='_build_debug'
[ "$dir" = "_build_doxygen" ] && exec sh -c 'doxygen' # && cd _build_doxygen/latex && make pdf'
[ -d "$dir" ] || {
    [ "$dir" == "_build_release" ] && mode=RelWithDebInfo ||
    [ "$dir" == "_build_debug" ]   && mode=Debug ||
    [ -n "$2" ] && mode="$2" ||
    echo "Using default build mode: Debug" && mode=Debug
    CMAKE_BUILD_TYPE="$mode" cmake -B "$dir" -S .
}
cmake --build "$dir" &&
{
    ctest --test-dir "$dir" -R '.*sygaldry.*' ||
        ctest --test-dir "$dir" --rerun-failed --output-on-failure
}
