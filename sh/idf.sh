#!/bin/sh -e

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

./sh/lili.sh || exit 1

# relies on environment variables set in nix-shell shellHook
# TODO: check that they are set reasonably and complain otherwise

[ -d "$IDF_PATH" ] || {
    git clone https://github.com/espressif/esp-idf.git "$IDF_PATH"
    pushd "$IDF_PATH"
        git fetch -a
        git checkout v5.1
    popd
    "./$IDF_PATH/install.sh"
}
source "$IDF_PATH/export.sh"

cd "sygaldry-instruments/$1"
shift
idf.py $@
