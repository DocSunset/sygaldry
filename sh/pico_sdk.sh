#!/usr/bin/env sh

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

"$SYGALDRY_ROOT/sh/lili.sh" || exit 1

export PICO_SDK_PATH="$SYGALDRY_ROOT/nixenv/pico_sdk"
echo "pico_sdk.sh -- PICO_SDK_PATH: $PICO_SDK_PATH"

[ -d "$PICO_SDK_PATH" ] || {
    echo "pico_sdk.sh -- PICO_SDK_PATH '$PICO_SDK_PATH' is not a directory; installing pico SDK..."
    git clone https://github.com/raspberrypi/pico-sdk.git "$PICO_SDK_PATH"
    pushd "$PICO_SDK_PATH"
        git fetch -a
        git checkout 1.5.1
        git submodule update --init
    popd
}

cd "$SYGALDRY_ROOT/sygaldry-instruments/$1"
shift
[ -d "_build_release" ] || CMAKE_BUILD_TYPE="RelWithDebInfo" cmake -B "_build_release" -S .
cmake --build "_build_release" -j 4
