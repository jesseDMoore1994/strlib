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
          pkgs.clang-tools
          pkgs.valgrind
        ];
        buildPhase = "make && valgrind --leak-check=yes ./dist/test";
        installPhase = ''
          mkdir -p $out/bin $out/lib
          mv dist/test $out/bin
          mv dist/strlib.so $out/lib
        '';
      };
  in rec {
    defaultApp = inputs.utils.lib.mkApp { drv = defaultPackage; };
    defaultPackage = strlib;
    devShell = pkgs.mkShell {
      buildInputs = [ pkgs.valgrind pkgs.clang ];
    };
  });
}
