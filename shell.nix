let pkgs = import (fetchTarball {
        url = https://github.com/NixOS/nixpkgs/archive/refs/tags/23.05.tar.gz;
        sha256 = sha256:10wn0l08j9lgqcw8177nh2ljrnxdrpri7bp0g7nvrsn9rkawvlbf;
    }) { overlays = [ (import ./nix/overlay.nix) ]; }; # apply overlay to make lili available
in pkgs.mkShell {
        name = "sygaldry";
        packages = [
            pkgs.git # required so cmake can fetch git repos like catch2
            pkgs.cacert # required so cmake can fetch git repos like catch2
            pkgs.liblo # for building OSC bindings tests. TODO this should be optional
            pkgs.pkg-config # so cmake can find liblo
            pkgs.cmake
            pkgs.doxygen
            pkgs.parallel
            pkgs.lili
        ];
}
