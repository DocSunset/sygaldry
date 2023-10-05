#!/bin/sh -e

# Update the documentation repo at $1

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT
sh/run.sh "_build_doxygen"
docs="_build_doxygen/html"
[ -d "$1" ] && repo="$1" || repo="../sygaldry-docs"
[ -d "$repo" ] || { echo "could not find documentation repository $repo" ; exit 1 ; }
hash="$(git rev-parse HEAD)"
rm -r "$repo/html"
cp -r "$docs" "$repo"
cd "$repo"
git commit --all -m "Update to sygaldry commit $hash (modulo working tree changes)"
git push
