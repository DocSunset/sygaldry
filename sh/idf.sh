#!/bin/env sh

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

"$SYGALDRY_ROOT/sh/lili.sh" || exit 1

export IDF_TOOLS_PATH="$SYGALDRY_ROOT/nixenv/esp-idf-tools"
mkdir -p "$IDF_TOOLS_PATH"
export IDF_PATH="$SYGALDRY_ROOT/nixenv/esp-idf"
echo "idf.sh -- IDF_PATH: $IDF_PATH    IDF_TOOLS_PATH: $IDF_TOOLS_PATH"

[ -d "$IDF_PATH" ] || {
    echo "idf.sh -- IDF_PATH '$IDF_PATH' is not a directory; installing esp-idf..."
    git clone https://github.com/espressif/esp-idf.git "$IDF_PATH"
    pushd "$IDF_PATH"
        git fetch -a
        git checkout v5.1
    popd
    "$IDF_PATH/install.sh"
}
source "$IDF_PATH/export.sh"

cd "$SYGALDRY_ROOT/sygaldry-instruments/$1"
shift
idf.py $@
