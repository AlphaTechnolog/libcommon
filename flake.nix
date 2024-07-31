{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux";

    pkgs = import nixpkgs {
      inherit system;

      overlays = [
        (_: final: {
          libcommon = final.callPackage ./default.nix {
            pkgs = final;
          };
        })
      ];
    };
  in {
    packages.${system} = {
      default = self.packages.${system}.libcommon;

      inherit (pkgs)
        libcommon
      ;
    };

    devShells.${system}.default = pkgs.mkShell {
      nativeBuildInputs = [ pkgs.libcommon ];

      LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
        pkgs.libcommon
      ];
    };
  };
}
