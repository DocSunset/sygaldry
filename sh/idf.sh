#!/bin/sh -e
sygaldry_root="$(pwd)"
cd "$1"
idf.py "$2" -D SYGALDRY_ROOT="$sygaldry_root"
