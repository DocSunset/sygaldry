\page page-sh-generate_components_cmake generate_components_cmake.sh Automated CMake Component List

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This script automatically generates a CMake file that weaves together all of the
components in the `sygaldry` directory, based on their component identifiers.
This saves contributors from having to manually maintain this list.
See \ref page-docs-build_system for more details.

The script is expected to be called appropriately by the convenience scripts;
it is currently hooked into [the `lili` script](\ref page-sh-lili), since that
script is required to be run before building anything, which is the appropriate
time to run this generation step.

This script contains the authoritative specification of what package groups
exist. In order to create a new package group, it must be added to the heredoc
that is used to initialize the list of components below.

The procedure is as follows:

- generate the empty skeleton of the file, with all of the package groups
- get a list of components
- for each component:
    - add the component to the file at the appropriate location

```sh
# @#'generate_components_cmake.sh'
#!/usr/bin/env sh

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

# this should be added in the top level CMakeLists.txt with add_subdirectory
# see \ref page-docs-build_system
cmakeliststxt="$SYGALDRY_ROOT/sygaldry/CMakeLists.txt"

# generate skeleton list file including package groups
cat << LISTSFILE > "$cmakeliststxt"
function(syg_add_package_group group)
    add_library(\${group} INTERFACE)
    target_link_libraries(sygaldry INTERFACE \${group})
endfunction()

function(syg_add_component component parent)
    add_subdirectory(\${component})
    if(TARGET \${component})
        target_link_libraries(\${parent} INTERFACE \${component})
    endif()
endfunction()

add_library(sygaldry INTERFACE)

syg_add_package_group(sygac)
syg_add_package_group(sygah)
syg_add_package_group(sygup)
syg_add_package_group(sygsp)
syg_add_package_group(sygbp)

if (ESP_PLATFORM)
syg_add_package_group(syghe)
syg_add_package_group(sygsa)
syg_add_package_group(sygse)
syg_add_package_group(sygbe)
endif()

if (PICO_SDK)
syg_add_package_group(sygbr)
syg_add_package_group(sygsa)
syg_add_package_group(sygsr)
endif()
LISTSFILE

linecount="$(cat "$cmakeliststxt" | wc -l)"
componentcount="$(list_components | wc -l)"
expected_linecount="$(echo "$linecount+$componentcount" | bc)"

# for each component found with `find`:
list_components | while read -r component ; do
        package_group="${component%%-*}"
        # add the necessary lines
        sed -i -e "/syg_add_package_group($package_group)/asyg_add_component($component $package_group)" "$cmakeliststxt"
    done

# check that each component found added one line to the skeleton
actual_linecount="$(cat "$cmakeliststxt" | wc -l)"
[ "$expected_linecount" -eq "$actual_linecount" ] ||
    echo "there are fewer components added to $cmakeliststxt than expected; is there a new package group? See $0 for more information"
# @/
```
