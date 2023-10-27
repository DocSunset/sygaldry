\page page-docs-build_system The Build System

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes how to set up the development environment for Sygaldry,
as well as the convenience scripts provided to facilitate development.

# Important Note

If you have any trouble setting up your development environment, please create
an issue on the github repository.

Don't clone the repo yet unless you're sure you know how to recursively clone
all its submodules. Otherwise, we'll get to that step in a minute.

# Overview

The process, in brief, is as follows:

1. Install `nix` according to the upstream documentation
[here](https://nixos.org/download), or that of your operating system
distribution.
2. Obtain a copy of the Sygaldry source code repository *and its submodules, recursively*,
using `git`, e.g. `git clone --recurse-submodules url-or-ssh-path/to/DocSunset/sygaldry`.
3. Use `nix-shell` to automatically install all dependencies, run the test-suite,
and compile instrument firmware.

# Basic Development Environment

## macOS

Run the `terminal` app and try to run `git --version`. If you have not already
installed developer tools, follow the instructions to do so.

## Linux

Most likely there is nothing required for you to do here.

## Windows

Windows support is still a work in progress. Please try installing a VirtualBox
VM with your preferred Linux distribution.

# Installing Nix

## Linux

Your distribution may have specific instructions for installing `nix`. Check the
documentation.

## Otherwise

Navigate to https://nixos.org/download and follow the upstream install instructions.

### What's Nix?

We use `nix` in this project because it is a convenient way of allowing all
users to quickly set up an identical development environment. Don't worry too
much about how this works, as the use of `nix` in this project is very limited
and simple to achieve. But if you're curious anyways...

Nix is a package manager, like `homebrew`, `apt`, `pacman`, or `pip`, but
rather than installing everything directly in your system directories, `nix`
installs things in "the Nix store", with every package in its own
content-hashed isolated directory. Commands like `nix-shell` can then be used
to draw these isolated directories together into a highly controlled
development environment that can be easily replicated across different
machines. You may think of it as a cross between a package manager and python
`venv`. Alternatively, if a container system like `docker` is a more
lightweight version of a virtual machine, you could think of `nix` as providing
a more lightweight version of a container system.

# Cloning Sygaldry

While Sygaldry is still a private repository, this step can be somewhat
involved, especially for less experienced developers. You will require a github
user account and permission to access the repository (ask Travis).

Once you have access to the repository, you can clone it.

If you are familiar with the use of git, make sure to clone the repository
*with all its submodules*, e.g. `git clone --recurse-submodules
https://github.com/DocSunset/sygaldry.git`. Once the repo is cloned, continue with
the next step.

If you are unfamiliar with the use of git, open your terminal and run
`nix-shell -p github-cli`. Nix will install the github command line interface
and drop you into a `nix` virtual environment in which it is accessible.

You now have a few options:

- use the github command line interface (aka `gh`) with https
- use the github command line interface with ssh
- use `git` with ssh

## Using the github command line interface with https

Run `gh auth login` from the `nix-shell` you enabled in the previous section.
Choose `HTTPS` as your preferred protocol for git operations, and then follow
the instructions to log in.

Once you are logged in, run `gh repo clone DocSunset/sygaldry -- --recurse-submodules`
to clone the repository and its submodules.

Continue with the convenience scripts.

## Using the github command line interface with ssh authentication

In the `nix-shell` that you enabled in the previous section, run `ssh-keygen -t
ed25519` and follow the onscreen instructions to create an ssh key pair. This
will create a private key and a public key. You should never share your private
key, but it is normal to upload your public key to someone else's server.

Run `gh auth login` from the `nix-shell` you enabled in the previous section.
Choose `SSH` as your preferred protocol for git operations, and then follow
the instructions to log in.

Once you are logged in, run `gh repo clone DocSunset/sygaldry -- --recurse-submodules`
to clone the repository and its submodules.

## Using git with ssh authentication

In the `nix-shell` that you enabled in the previous section, run `ssh-keygen -t
ed25519` and follow the onscreen instructions to create an ssh key pair. Use the
default name and path for the new keys, unless you know what you are doing. This
will create a private key and a public key. You should never share your private
key, but it is normal to upload your public key to someone else's server.

Use `cat` to display the public key, e.g. `cat /home/myuser/.ssh/id_ed25519.pub`.
Copy the public key to your clipboard.

Log in to github in a web browser and navigate to Settings, then "SSH and GPG Keys".
Click the green button to add a "New SSH key". Give it a title such as the hostname
of your computer. Select "Authentication Key" as the key type. In the "Key" text field,
paste the public key. Click "Add SSH key".

In the terminal, run `git clone --recurse-submodules
git<AT>github.com:DocSunset/Sygaldry.git`, with an at-sign where it says `<AT>`
to clone the repository and its submodules.

# Ready to go!

`cd` your terminal to the root of the cloned repository and run `nix-shell
--pure`. This may take a little while the first time, while `nix` sets up the
development environment and the `esp-idf` is installed.

Once you reach the command prompt, you are now ready to go! Run `./sh/run.sh`
to compile and run the tests, `doxygen` to generate local documentation,
`./sh/idf.sh t_stick app` to compile the T-Stick firmware, and so on.

Alternatively, rather than running `nix-shell --pure` to drop into the sygaldry
development environment, you may also run `nix-shell --pure --run "..."` with a
command such as `sh/run.sh` as the argument to the `--run` flag. This allows
you to run a single command from the sygaldry development environment without
fully switching your terminal to it.

The remainder of this document provides supplementary information on the
convenience scripts. The curious may read on. Otherwise, you may continue
with another guide.

# Nix shell implementation details

We have three nix expressions. The first one packages `lili` so that you don't
have to manually install it. This is fairly trivial using the standard builder
from `nixpkgs.stdenv`:

```nix
# @#'nix/lili.nix'
# derivation for lili
{ pkgs }: pkgs.stdenv.mkDerivation {
    name = "lili";
    src = pkgs.fetchFromGitHub {
        owner = "DocSunset";
        repo = "lili";
        rev = "main";
        sha256 = "sha256-2AeLHCwtUK/SZMhhlRcDvtJdKtkouUfofm7Bg6+FcAc=";
    };
    installPhase = ''
        make prefix="$out" install
    '';
}
# @/
```

We then provide an overlay that allows us to make our `lili` package available
as an extension of `nixpkgs`.

```nix
# @#'nix/overlay.nix'
final: prev: {
    lili = prev.callPackage ./lili.nix {}; # paths are relative to the dir the file is in
}
# @/
```

Finally, we can define our shell by importing a pinned version of `nixpkgs`,
overlaying our `lili` package, and declaring our dependencies. The shell hook
is used to additionally install `esp-idf` using the upstream manual install
method, i.e. by cloning the `esp-idf` repository and running its included
`install.sh`.

```nix
# @#'shell.nix'
let pkgs = import (fetchTarball {
        url = https://github.com/NixOS/nixpkgs/archive/refs/tags/23.05.tar.gz;
        sha256 = sha256:10wn0l08j9lgqcw8177nh2ljrnxdrpri7bp0g7nvrsn9rkawvlbf;
    }) { overlays = [ (import ./nix/overlay.nix) ]; }; # apply overlay to make lili available
in pkgs.stdenvNoCC.mkDerivation {
        name = "sygaldry";
        nativeBuildInputs = [
            pkgs.gcc13 # default compiler
            pkgs.git # required so cmake can fetch git repos like catch2. Also for esp-idf, Pi Pico SDK
            pkgs.cacert # required so cmake can fetch git repos like catch2
            pkgs.pkg-config # so cmake can find liblo
            pkgs.cmake # main build automation tool; required for esp-idf, Pi Pico SDK
            pkgs.doxygen # used to build documentation website
            pkgs.parallel # used to speed up helper scripts
            pkgs.lili # literate programming

            # additional packages required for esp-idf
            pkgs.wget
            pkgs.gnumake
            pkgs.flex
            pkgs.bison
            pkgs.gperf
            pkgs.python3 # possibly also used by Pi Pico SDK
            pkgs.ninja # possibly also used by Pi Pico SDK
            pkgs.ccache
            pkgs.dfu-util

            # additional packages required for Pi Pico SDK
            pkgs.gcc-arm-embedded
            pkgs.openocd
        ];
        buildInputs = [
            pkgs.boost # required by Avendish
            pkgs.liblo # for building OSC bindings tests. TODO this should be optional
            pkgs.puredata # for building pd externals with Avendish
        ];
        shellHook = ''
            export SYGALDRY_ROOT="${toString ./.}"
            mkdir -p "$SYGALDRY_ROOT/nixenv/"

            export IDF_TOOLS_PATH="$SYGALDRY_ROOT/nixenv/esp-idf-tools"
            mkdir -p "$IDF_TOOLS_PATH"
            export IDF_PATH='nixenv/esp-idf'

            export PICO_SDK_PATH="$SYGALDRY_ROOT/nixenv/pico_sdk"
        '';
}
# @/
```

# Convenience Scripts

## Literate Sources

Literate programming is employed throughout the project. The literate sources
are considered authoritative, and are used to generate most other source files,
including many CMakeLists.txt files.

The following shell script can regenerate all machine sources from the lili
literate source files, and it (or something equivalent) should be run before
every build.

In addition, if `lili` is not already installed, the script will attempt to
install it.

For each `.lili.md` file found in the repository with `find`, the script finds
the first file generated by that literate source, and if the source is more
recently modified than the generated file, reruns `lili` to regenerate all
files derived from that literate source. GNU `parallel` is used to perform this
subroutine for all literate sources. The command exits with a failure code in
case any invocation of `lili` fails, which is used to short-circuit execution
of later shell scripts.

Run this script from the root of the repository.

```sh
# @#'sh/lili.sh'
#!/bin/sh -e
# generate machine sources from literate source code

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

cd "$SYGALDRY_ROOT"
find -iname '*.lili.md' | parallel '
    cd {//}
    generated="$(grep -m 1 -h -r -I "@@#" {/} | head -n 1 | sed 's/^.*@#.//' | sed 's/.$//')"
    [ "$generated" ] && [ {/} -nt "$generated" ] &&
    {
        echo {} "newer than $generated, running lili..."
        lili {/} || exit 1 # short circuit on any lili error with failure return code
    } || exit 0
'
# @/
```

### ccache

One limitation of this current approach to literate tangle synchronisation is
that, if any file generated by a given literate source document is changed,
then all files generated by that literate source are regenerated. This poorly
impacts incremental builds, sometimes causing unnecessary recompilation of
machine sources that have not actually changed due to their updated access
times. It is recommended to use [`ccache`](https://ccache.dev/) to work around
this issue, and generally speed up build times.

TODO: add ccache to the nix setup.

## Doxygen

In addition to the literate sources, which are intended to document the
implementation and design rationale of the project, public API's are also
documented using Doxygen special comment blocks, and pretty documentation is
generated with Doxygen and Doxygen Awesome CSS.

The order in which files are presented in the generated documentation is
controlled by manually specifying each file as a subpage in
[the implementation guide](\ref docs-implementation),
as well as some pages being ordered depending
on their order in the `doxyfile` `INPUT` option.

Documentation can be generated e.g. by running `doxygen` in the root of the
repository (remember to run `lili.sh` first!), or using the `run.sh
_build_doxygen`.

The documentation website is served from a seperate repository. You can update
the documentation website repository with the following script which takes the
sygladry documentation repo's directory as argument, defaulting to
`../sygaldry-docs`.

```sh
# @#'sh/update_docs.sh'
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
# @/
```

## Building

A simple shell script is provided to run the compiler and test suite, or
generate documentation, useful for development. Run it from the root of the
project passing the name of the build directory as first argument. The
directories `_build_debug`, `_build_release`, and `_build_doxygen` are
accounted for and included in `.gitignore`.

```sh
# @#'sh/run.sh'
#!/bin/sh -e

# create the build directory if necessary, then build and test the project

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

"$SYGALDRY_ROOT/sh/lili.sh" || exit 1
[ "$#" -gt 0 ] && dir="$1" || dir='_build_debug'
[ "$dir" = "_build_doxygen" ] && exec sh -c 'doxygen' # && cd _build_doxygen/latex && make pdf'
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

### Building with Clang

Assuming you are building on a machine where `gcc` is the default compiler, you
may wish to compile using `clang` e.g. in order to check for compiler-specific
incompatibilities. You may be able to accomplish this using environment
variables when generating the build directory:

```sh
CC=/usr/bin/clang CXX=/usr/bin/clang++ cmake -D_CMAKE_TOOLCHAIN_PREFIX=llvm -B _build_clang -S .
```

Presumably a similar tactic could be used to compile using `gcc` on a machine where
`clang` is the default, but this hasn't been tested.

## Platform Specific

### ESP32

Building an ESP32 instrument is currently achieved using the normal ESP-IDF
build tools. The following POSIX shell script would run `idf.py` for the ESP32
instrument located in the directory passed as the first argument to the script,
forwarding remaining arguments to idf.py. The script will also clone, install,
and export the IDF if it is not already available.

```sh
# @#'sh/idf.sh'
#!/bin/sh -e

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

"$SYGALDRY_ROOT/sh/lili.sh" || exit 1

# relies on environment variables set in nix-shell shellHook
# TODO: check that they are set reasonably and complain otherwise

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

## Raspberry Pi Pico SDK

Building for Raspberry Pi Pico requires a copy of the Pico C/C++ SDK. The following
POSIX shell script ensures that the SDK is available, before changing directory
to the given instrument and invoking CMake to build the firmware for it.

```sh
# @#'sh/pico_sdk.sh'
#!/bin/sh -e

# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
# Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
# Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
# Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

"$SYGALDRY_ROOT/sh/lili.sh" || exit 1

# relies on environment variables set in nix-shell shellHook
# TODO: check that they are set reasonably and complain otherwise

echo "pico_sdk.sh -- PICO_SDK_PATH: $PICO_SDK_PATH"

[ -d "$PICO_SDK_PATH" ] || {
    echo "pico_sdk.sh -- PICO_SDK_PATH '$PICO_SDK_PATH' is not a directory; installing pico SDK..."
    git clone https://github.com/raspberrypi/pico-sdk.git "$PICO_SDK_PATH"
    pushd "$PICO_SDK_PATH"
        git fetch -a
        git checkout 1.5.1
        git submodule update --init
    popd
}

cd "$SYGALDRY_ROOT/sygaldry-instruments/$1"
shift
[ -d "_build_release" ] || CMAKE_BUILD_TYPE="RelWithDebInfo" cmake -B "_build_release" -S .
cmake --build "_build_release" -j 4
# @/
```

# The CMakeLists.txt

## Sygaldry root

We set a variable allowing components to reach the root of the repository. This
is mainly done so that components can reach the `dependencies` directory
unambiguously as `${SYGALDRY_ROOT}/dependencies`, rather than using a relative
path that might break if the component is physically moved.

```cmake
# @='set SYGALDRY_ROOT'
set(SYGALDRY_ROOT ${CMAKE_CURRENT_LIST_DIR})
# @/
```

## Language Standard

We require C++20 without extensions.

```cmake
# @='set language standard'
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)
# @/
```

## Testing Framework

Catch2 is used for unit testing on platforms where it can run. Other platforms
may use a different testing methodology.

```cmake
# @='include FetchContent'
Include(FetchContent)
# @/

# @='Fetch Catch2'
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2 # or a later release
)

FetchContent_MakeAvailable(Catch2)
# @/

# @='Include automatic test registration'
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
# @/
```

Catch2 is not a good testing framework for ESP32, so the code to enable it
is disabled for that platform.

```cmake
# @='prepare for tests'
if (NOT ESP_PLATFORM AND NOT PICO_SDK)
@{Fetch Catch2}
@{Include automatic test registration}
set(SYGALDRY_BUILD_TESTS 1)
endif()
# @/
```

See [the Catch2 CMake integration documentation](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#top) for more detail.

## Avendish

Avendish is used to build Pd externals, and eventually perhaps for other
bindings.

```cmake
# @='fetch Avendish'
if (NOT ESP_PLATFORM AND NOT PICO_SDK)
FetchContent_Declare(
  avendish
  GIT_REPOSITORY "https://github.com/celtera/avendish"
  GIT_TAG  3b3bd7b2ecf2061900726100e664b69c51b8e402
  GIT_PROGRESS true
)
FetchContent_Populate(avendish)

set(CMAKE_PREFIX_PATH "${avendish_SOURCE_DIR};${CMAKE_PREFIX_PATH}")
find_package(Avendish REQUIRED)
set(SYGALDRY_BUILD_AVENDISH 1)
endif()
# @/
```

## CMake Enabled Libraries

Boost PFR and Boost MP11 are required by the concepts library, and consequently
by any bindings or components that make use of it. Several other components
make use of specific libraries, such as liblo and `Trill-Arduino`; some of
these are also included as submodules, and some may be required to build the
default test suite. Since many of these dependencies do not provide cmake
support, they are handled on a case by case basis in the `CMakeLists.txt` files
of the components that use them. Those that can be included via CMake are made
available here, when appropriate depending on the platform.

```cmake
# @='include cmake libraries'
add_subdirectory(dependencies/pfr)
add_subdirectory(dependencies/mp11)
if(PICO_SDK)
    # on Raspberry Pi Pico SDK, the upsteam Eigen3 cmake trips an error,
    # see https://gitlab.com/libeigen/eigen/-/issues/2740
    # for now, we work around this by manually defining the Eigen library with
    # a compatible name, alias, etc.
    set ( EIGEN_DEFINITIONS "")
    add_library (eigen INTERFACE)
    add_library (Eigen3::Eigen ALIAS eigen)
    target_compile_definitions (eigen INTERFACE ${EIGEN_DEFINITIONS})
    target_include_directories (eigen INTERFACE $ENV{SYGALDRY_ROOT}/dependencies/eigen)
else()
    set(EIGEN_BUILD_TESTING FALSE)
    add_subdirectory(dependencies/eigen)
endif()
# @/
```

## Software Components

The project is physically structured as self-contained software components, in
the sense described by John Lakos \cite lakos2019large-scale-cpp. Each
component lives in its own directory, with its own CMakeLists.txt file. The
main CMakeLists.txt file's principal job is to include these components as
subdirectories.

In an earlier version of the project, there was a directory structure
resembling the following, where each leaf node was a directory containing a
component as just described:

```
sygaldry
*-- bindings
|   *-- esp32
|   |   *-- libmapper-arduino
|   |   *-- spiffs
|   |   *-- etc.
|   *-- portable
|       *-- cli
|       *-- output_logger
|       *-- etc.
*-- concepts
|   *-- components
|   *-- endpoints
|   *-- etc.
*-- helpers
|   *-- endpoints
|   *-- metadata
|   *-- etc.
*-- sensors
    *-- arduino
    |   *-- icm20948
    |   *-- trill_craft
    |   *-- etc.
    *-- esp32
    |   *-- adc
    |   *-- arduino-hack
    |   *-- etc.
    *-- portable
        *-- etc.
```

Components were correspondingly named e.g. `sygaldry-sensors-esp32-adc`.

As well as being troublingly verbose when editing, this proved problematic when
building on Windows, which at the time in the year 2023 enforced a limit of 260
characters on the length of a path when making a directory. Some instrument
firmwares being built at the time would include the main repository root as a
library, resulting in very long build artefact paths such as
`C:\Users\user\Github\sygaldry\sygaldry\instruments\instrument_name\instrument_name_platform\instrument\build\main\sygbuild\C_Users\user\Github\sygaldry\sensors\arduino\trill_craft\sygaldry-sensors-arduino-trill_craft.obj.d`
that would eventually trip the path length limit. Although it was possible to
modify registry keys and otherwise work around the issue, given the
inconvenience entailed in editing and maintaining the long component names, it
was decided to adopt an organizational scheme with better characteristics in
terms of brevity.

The repository now contains two main content directories: `sygaldry` and
`sygaldry-instruments`. The `sygaldry` directory contains all of the software
components in the library, except for the instruments, which are unsurprisingly
found in the other directory. Rather than being named verbosely, in keeping
with \cite lakos2019large-scale-cpp, packages and their components are given
identifiers of the form `sygXY` where `XY` is the package identifier, with `X`
typically (currently always) referring to the broad type of components and `Y`
to the platform.

```cmake
# @='add subdirectories'
add_library(sygaldry INTERFACE)

add_library(sygac INTERFACE)
                add_subdirectory(sygaldry/sygac-components)
    target_link_libraries(sygac INTERFACE sygac-components)
                add_subdirectory(sygaldry/sygac-endpoints)
    target_link_libraries(sygac INTERFACE sygac-endpoints)
                add_subdirectory(sygaldry/sygac-functions)
    target_link_libraries(sygac INTERFACE sygac-functions)
                add_subdirectory(sygaldry/sygac-metadata)
    target_link_libraries(sygac INTERFACE sygac-metadata)
                add_subdirectory(sygaldry/sygac-mimu)
    target_link_libraries(sygac INTERFACE sygac-mimu)
                add_subdirectory(sygaldry/sygac-runtime)
    target_link_libraries(sygac INTERFACE sygac-runtime)
target_link_libraries(sygaldry INTERFACE sygac)

add_library(sygah INTERFACE)
                add_subdirectory(sygaldry/sygah-consteval)
    target_link_libraries(sygah INTERFACE sygah-consteval)
                add_subdirectory(sygaldry/sygah-endpoints)
    target_link_libraries(sygah INTERFACE sygah-endpoints)
                add_subdirectory(sygaldry/sygah-string_literal)
    target_link_libraries(sygah INTERFACE sygah-string_literal)
                add_subdirectory(sygaldry/sygah-metadata)
    target_link_libraries(sygah INTERFACE sygah-metadata)
                add_subdirectory(sygaldry/sygah-mimu)
    target_link_libraries(sygah INTERFACE sygah-mimu)
target_link_libraries(sygaldry INTERFACE sygah)

add_library(sygsp INTERFACE)
    # arduino-hack, alphabetically missing here, is added only where required by a platform
                add_subdirectory(sygaldry/sygsp-button)
    target_link_libraries(sygsp INTERFACE sygsp-button)
                add_subdirectory(sygaldry/sygsp-delay)
    target_link_libraries(sygsp INTERFACE sygsp-delay)
                add_subdirectory(sygaldry/sygsp-micros)
    target_link_libraries(sygsp INTERFACE sygsp-micros)
                add_subdirectory(sygaldry/sygsp-mimu_units)
    target_link_libraries(sygsp INTERFACE sygsp-mimu_units)
                add_subdirectory(sygaldry/sygsp-complementary_mimu_fusion)
    target_link_libraries(sygsp INTERFACE sygsp-complementary_mimu_fusion)
target_link_libraries(sygaldry INTERFACE sygsp)

if (ESP_PLATFORM)
    add_library(sygse INTERFACE)
    add_library(sygsa INTERFACE)
                    add_subdirectory(sygaldry/sygsp-arduino_hack)
        target_link_libraries(sygsp INTERFACE sygsp-arduino_hack)
                    add_subdirectory(sygaldry/sygse-arduino_hack)
        target_link_libraries(sygse INTERFACE sygse-arduino_hack)
                    add_subdirectory(sygaldry/sygsa-two_wire)
        target_link_libraries(sygsa INTERFACE sygsa-two_wire)
                    add_subdirectory(sygaldry/sygsa-trill_craft)
        target_link_libraries(sygsa INTERFACE sygsa-trill_craft)
                    add_subdirectory(sygaldry/sygse-delay)
        target_link_libraries(sygsa INTERFACE sygse-delay)
                    add_subdirectory(sygaldry/sygse-micros)
        target_link_libraries(sygsa INTERFACE sygse-micros)
                    add_subdirectory(sygaldry/sygse-adc)
        target_link_libraries(sygse INTERFACE sygse-adc)
                    add_subdirectory(sygaldry/sygse-button)
        target_link_libraries(sygse INTERFACE sygse-button)
                    add_subdirectory(sygaldry/sygse-gpio)
        target_link_libraries(sygse INTERFACE sygse-gpio)
                    add_subdirectory(sygaldry/sygsp-icm20948)
        target_link_libraries(sygse INTERFACE sygsp-icm20948)
                    add_subdirectory(sygaldry/sygse-trill)
        target_link_libraries(sygse INTERFACE sygse-trill)
    target_link_libraries(sygaldry INTERFACE sygse)
    target_link_libraries(sygaldry INTERFACE sygsa)
endif()

add_library(sygbp INTERFACE)
                add_subdirectory(sygaldry/sygbp-cstdio_reader)
    target_link_libraries(sygbp INTERFACE sygbp-cstdio_reader)
                add_subdirectory(sygaldry/sygbp-cli)
    target_link_libraries(sygbp INTERFACE sygbp-cli)
                add_subdirectory(sygaldry/sygbp-liblo)
    target_link_libraries(sygbp INTERFACE sygbp-liblo)
                add_subdirectory(sygaldry/sygbp-osc_match_pattern)
    target_link_libraries(sygbp INTERFACE sygbp-osc_match_pattern)
                add_subdirectory(sygaldry/sygbp-osc_string_constants)
    target_link_libraries(sygbp INTERFACE sygbp-osc_string_constants)
                add_subdirectory(sygaldry/sygbp-output_logger)
    target_link_libraries(sygbp INTERFACE sygbp-output_logger)
                add_subdirectory(sygaldry/sygbp-rapid_json)
    target_link_libraries(sygbp INTERFACE sygbp-rapid_json)
                add_subdirectory(sygaldry/sygbp-session_data)
    target_link_libraries(sygbp INTERFACE sygbp-session_data)
                add_subdirectory(sygaldry/sygbp-spelling)
    target_link_libraries(sygbp INTERFACE sygbp-spelling)
                add_subdirectory(sygaldry/sygbp-test_component)
    target_link_libraries(sygbp INTERFACE sygbp-test_component)
    if (SYGALDRY_BUILD_TESTS)
                    add_subdirectory(sygaldry/sygbp-test_reader)
        target_link_libraries(sygbp INTERFACE sygbp-test_reader)
    endif()
target_link_libraries(sygaldry INTERFACE sygbp)

if (ESP_PLATFORM)
add_library(sygbe INTERFACE)

                add_subdirectory(sygaldry/sygbe-runtime)
    target_link_libraries(sygbe INTERFACE sygbe-runtime)
                add_subdirectory(sygaldry/sygbe-libmapper_arduino)
    target_link_libraries(sygbe INTERFACE sygbe-libmapper_arduino)
                add_subdirectory(sygaldry/sygbe-spiffs)
    target_link_libraries(sygbe INTERFACE sygbe-spiffs)
                add_subdirectory(sygaldry/sygbe-wifi)
    target_link_libraries(sygbe INTERFACE sygbe-wifi)

target_link_libraries(sygaldry INTERFACE sygbe)
endif()

if (PICO_SDK)
add_library(sygbr INTERFACE)
                add_subdirectory(sygaldry/sygbr-runtime)
    target_link_libraries(sygbr INTERFACE sygbr-runtime)
                add_subdirectory(sygaldry/sygbr-cli)
    target_link_libraries(sygbr INTERFACE sygbr-cli)
target_link_libraries(sygaldry INTERFACE sygbr)
endif()

add_library(sygup INTERFACE)

                add_subdirectory(sygaldry/sygup-basic_logger)
    target_link_libraries(sygup INTERFACE sygup-basic_logger)
                add_subdirectory(sygaldry/sygup-cstdio_logger)
    target_link_libraries(sygup INTERFACE sygup-cstdio_logger)
    if (SYGALDRY_BUILD_TESTS)
                    add_subdirectory(sygaldry/sygup-test_logger)
        target_link_libraries(sygup INTERFACE sygup-test_logger)
    endif()

target_link_libraries(sygaldry INTERFACE sygup)

add_subdirectory(sygaldry-instruments/test)
# @/
```

# Summary

```cmake
# @#'CMakeLists.txt'
# Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music Interaction Laboratory
# (IDMIL), Centre for Interdisciplinary Research in Music Media and Technology
# (CIRMMT), McGill University, Montréal, Canada, and Univ. Lille, Inria, CNRS,
# Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.24)
project(Sygaldry)

@{set SYGALDRY_ROOT}
@{set language standard}
@{include FetchContent}

@{prepare for tests}

@{fetch Avendish}

@{include cmake libraries}

@{add subdirectories}
# @/
```
