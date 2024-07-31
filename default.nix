{ stdenv, pkgs, ... }: stdenv.mkDerivation {
  pname = "libcommon";
  version = "git";

  src = ./.;

  buildPhase = ''
    make
  '';

  installPhase = ''
    make PREFIX=$out install
  '';
}
