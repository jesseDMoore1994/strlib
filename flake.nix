{
  description = "A template for Nix based C project setup.";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs: inputs.utils.lib.eachSystem [
    "x86_64-linux" "i686-linux" "aarch64-linux" "x86_64-darwin"
  ] (system: let pkgs = import nixpkgs {
        inherit system;
      };
      strlib = pkgs.stdenv.mkDerivation {
        pname = "strlib";
        version = "0.1.0";
        src = ./.;
        nativeBuildInputs = [
          pkgs.gnumake
          pkgs.clang
        ];
        buildPhase = "make";
        installPhase = ''
          mkdir -p $out/bin
          mv test $out/bin
        '';
      };
  in rec {
    defaultApp = inputs.utils.lib.mkApp { drv = defaultPackage; };
    defaultPackage = strlib;
    devShell = pkgs.mkShell {
      buildInputs = [ strlib pkgs.valgrind pkgs.clang ];
    };
  });
}
