\page page-sh-test sh/test.sh Test Runner Convenience Script

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

A simple shell script is provided to compile and run the portable test suite,
or generate documentation. Run it from the root of the project passing the name
of the build directory as first argument. The directories `_build_debug`,
`_build_release`, and `_build_doxygen` are accounted for and included in
`.gitignore`.

Note that many components cannot be compiled in the nix shell environment,
since they depend on e.g. specific hardware devices and their associated
support libraries. These components must be tested seperately using
platform-specific methods. At the time of writing, thorough testing of
platform-specific components is still an early work in progress.

\warning All tests must have the string `sygaldry` in their name in order to be
picked up by this convenience script; this allows us to avoid running tests
that may be compiled by various dependencies.

```sh
# @#'test.sh'
#!/bin/sh -e

# create the build directory if necessary, then build and test the project

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

"$SYGALDRY_ROOT/sh/lili.sh" || exit 1
[ "$#" -gt 0 ] && dir="$1" || dir='_build_debug'
[ "$dir" = "_build_doxygen" ] && exec sh -c 'doxygen'
[ -d "$dir" ] || {
    [ "$dir" == "_build_release" ] && mode=RelWithDebInfo ||
    [ "$dir" == "_build_debug" ]   && mode=Debug ||
    [ -n "$2" ] && mode="$2" ||
    echo "Using default build mode: Debug" && mode=Debug
    CMAKE_BUILD_TYPE="$mode" cmake -B "$dir" -S .
}
cmake --build "$dir" -j 4 &&
{
    ctest --test-dir "$dir" -R '.*sygaldry.*' ||
        ctest --test-dir "$dir" --rerun-failed --output-on-failure
}
# @/
```
