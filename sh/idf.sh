#!/bin/sh -e

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

sygaldry_root="$(pwd)"
cd "$1"
idf.py "$2" -D SYGALDRY_ROOT="$sygaldry_root"
