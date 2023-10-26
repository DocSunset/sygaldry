let pkgs = import (fetchTarball {
        url = https://github.com/NixOS/nixpkgs/archive/refs/tags/23.05.tar.gz;
        sha256 = sha256:10wn0l08j9lgqcw8177nh2ljrnxdrpri7bp0g7nvrsn9rkawvlbf;
    }) { overlays = [ (import ./nix/overlay.nix) ]; }; # apply overlay to make lili available
in pkgs.stdenvNoCC.mkDerivation {
        name = "sygaldry";
        nativeBuildInputs = [
            pkgs.gcc13
            pkgs.git # required so cmake can fetch git repos like catch2. Also for esp-idf
            pkgs.cacert # required so cmake can fetch git repos like catch2
            pkgs.pkg-config # so cmake can find liblo
            pkgs.cmake
            pkgs.doxygen
            pkgs.parallel
            pkgs.lili

            # additional packages required for esp-idf
            pkgs.wget
            pkgs.gnumake
            pkgs.flex
            pkgs.bison
            pkgs.gperf
            pkgs.python3
            pkgs.ninja
            pkgs.ccache
            pkgs.dfu-util
        ];
        buildInputs = [
            pkgs.boost # required by Avendish
            pkgs.liblo # for building OSC bindings tests. TODO this should be optional
            pkgs.puredata # for building pd externals with Avendish
        ];
        shellHook = ''
            sh/lili.sh
            mkdir -p ./nixenv/esp-idf-tools/
            export IDF_TOOLS_PATH="$(realpath nixenv/esp-idf-tools)"
            export IDF_PATH='nixenv/esp-idf'
        '';
}
