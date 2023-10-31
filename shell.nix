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
