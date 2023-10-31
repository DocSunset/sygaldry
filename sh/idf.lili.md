\page page-sh-idf sh/idf.sh ESP-IDF Convenience Script

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

Building an ESP32 instrument is currently achieved using the normal ESP-IDF
build tools. The following POSIX shell script would run `idf.py` for the ESP32
instrument located in the directory passed as the first argument to the script,
forwarding remaining arguments to idf.py. The script will also clone, install,
and export the IDF if it is not already available.

```sh
# @#'idf.sh'
#!/bin/sh -e

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
idf.py $@@
# @/
```

Previously, the CMake `ExternalProject_Add` command was used to incorporate
the ESP32 instruments into the usual build process. However, this resulted
in long compilation times during development, even when the ESP32 targets
were not being tested. The current approach requires more manual intervention
from the developer, but is hoped to save time overall.

At the time of writing, Sygaldry also requires a fairly recent version of the
ESP-IDF; an appropriate version of the framework is installed when setting up
the development environment.
