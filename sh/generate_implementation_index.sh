#!/bin/env sh

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

# this should be added in the top level CMakeLists.txt with add_subdirectory
# see \ref page-docs-build_system
impldocpage="$SYGALDRY_ROOT/sygaldry/docs/implementation.md"

# generate skeleton index file including package groups
cat << LISTSFILE > "$impldocpage"
\page page-sygaldry-docs-implementation Implementation Documentation

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

The source files are annotated with literate prose meant to facilitate the
reader's understanding of the code. For more detailed documentation of the
library than the reference documentation provides, the literate sources are
the authoritative source on the behavior of the library.

See also \ref page-docs-contributing and \ref page-docs-making_a_new_component
for information on the coding conventions adopted throughout the project.

This document is partially automatically generated. See \ref page-sh-generate_implementation_index.

## Instruments
- \subpage page-sygin-t_stick

## Sensors

### Portable (sygsp)
- \subpage page-sygsp-byte_serif

### Arduino (sygsa)

### ESP-IDF (sygse)

### Raspberry Pi Pico SDK (sygsr)

## Utility Components (sygup)

## Bindings

### Portable (sygbp)
- \subpage page-sygbp-basic_reader

### ESP-IDF (sygbe)

### Raspberry Pi Pico SDK (sygbr)

## Helpers (sygah)

## Concepts (sygac)

## Infrastructure
- \subpage page-docs-build_system
LISTSFILE

linecount="$(cat "$impldocpage" | wc -l)"
componentcount="$(list_components | wc -l)"
expected_linecount="$(echo "$linecount+$componentcount" | bc)"

# for each component found with `find`:
list_components | while read -r component ; do
        package_group="${component%%-*}"
        # add the necessary lines
        sed -i -e '/('"$package_group"')/a- \\subpage page-'"$component" "$impldocpage"
    done

# check that each component found added one line to the skeleton
actual_linecount="$(cat "$impldocpage" | wc -l)"
[ "$expected_linecount" -eq "$actual_linecount" ] ||
    echo "there are fewer components added to $impldocpage than expected; is there a new package group? See $0 for more information"
