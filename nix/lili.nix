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
