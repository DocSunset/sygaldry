\page page-docs-nix_shell Nix Shell Setup

Copyright 2023 Travis J. West, https://traviswest.ca, Input Devices and Music
Interaction Laboratory (IDMIL), Centre for Interdisciplinary Research in Music
Media and Technology (CIRMMT), McGill University, Montréal, Canada, and Univ.
Lille, Inria, CNRS, Centrale Lille, UMR 9189 CRIStAL, F-59000 Lille, France

SPDX-License-Identifier: MIT

[TOC]

This document describes how the nix shell build environment is prepared.

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
overlaying our `lili` package, and declaring our dependencies. We favor
controlling dependencies via Nix, but note that several dependencies are also
included as git submodules in the `dependencies` subdirectory.

```nix
# @#'shell.nix'
let pkgs = import (fetchTarball {
        url = https://github.com/NixOS/nixpkgs/archive/refs/tags/23.05.tar.gz;
        sha256 = sha256:10wn0l08j9lgqcw8177nh2ljrnxdrpri7bp0g7nvrsn9rkawvlbf;
    }) { overlays = [ (import ./nix/overlay.nix) ]; }; # apply overlay to make lili available
in pkgs.stdenvNoCC.mkDerivation {
        name = "sygaldry";
        nativeBuildInputs = [ # dependencies at build time
            pkgs.gcc13 # default compiler
            pkgs.git # required so cmake can fetch git repos like catch2. Also for esp-idf, Pi Pico SDK
            pkgs.openssh # for convenience and in case the user uses ssh for git auth
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
        buildInputs = [ # dependencies at run time
            pkgs.catch2_3 # unit test library for portable tests
            pkgs.eigen # portable linear algebra library
            pkgs.boost # required by Avendish
            pkgs.liblo # for building OSC bindings tests. TODO this should be optional
            pkgs.puredata # for building pd externals with Avendish
        ];
        shellHook = ''
            export SYGALDRY_ROOT="${toString ./.}"
            mkdir -p "$SYGALDRY_ROOT/nixenv/"
            export PATH="$SYGALDRY_ROOT/sh:$PATH"
        '';
}
# @/
```

Notice that the shell hook is used to create the directory `nixenv` and export
the environment variable `SYGALDRY_ROOT`. These are used extensively by
convenience scripts, as well as the CMake build automation.
