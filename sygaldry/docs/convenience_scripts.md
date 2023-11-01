\page page-docs-convenience_scripts Convenience Scripts

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

Several convenience scripts are provided to facilitate development. When
[the nix shell environment](\ref page-docs-developer_setup) is activated
by running `nix-shell --pure`, these can be run directly, e.g. `test.sh`, since
the `sh` directory is prepended to the binary search path. Alternatively, you
may run `nix-shell --pure --run 'script.sh'` to run a given script without
dropping into the nix shell environment.

- \subpage page-sh-lili
    - Run `lili` on each literate source file that has been update, regenerating tangled machine code
    - This script automatically calls the following scripts that generally needn't be called directly:
        - \subpage page-sh-generate_components_cmake
        - \subpage page-sh-generate_implementation_index
- \subpage page-sh-test
    - Compile and run tests for portable components.
- \subpage page-sh-new_component
    - Automatically generate the skeleton of a new component. See \ref page-docs-making_a_new_component for more details.
- \subpage page-sh-update_docs
    - Update the doxygen documentation website repository
- \subpage page-sh-idf
    - Install the ESP-IDF and build an IDF-based instrument firmware.
- \subpage page-sh-pico_sdk
    - Install the Raspberry Pi Pico SDK and build a Pico SDK-based instrument firmware.
