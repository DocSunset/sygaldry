#!/usr/bin/env sh
# generate machine sources from literate source code

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

cd "$SYGALDRY_ROOT"
find -iname '*.lili.md' | parallel '
    cd {//}
    generated="$(grep -m 1 -h -r -I "@#" {/} | head -n 1 | sed 's/^.*@#.//' | sed 's/.$//')"
    [ "$generated" ] && [ {/} -nt "$generated" ] &&
    {
        echo {} "newer than $generated, running lili..."
        lili {/} || exit 1 # short circuit on any lili error with failure return code
    } || exit 0
' && generate_components_cmake.sh
