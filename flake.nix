{
  description = "Gradle C++ cross-compile (Linux → Windows/Mac)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          name = "CppShell";

          nativeBuildInputs = with pkgs; [
            gcc15Stdenv
            gcc15
            ninja
            cmake
            clang-tools
          ];

          buildInputs = with pkgs; [
          ];

          shellHook = '''';
        };
      }
    );
}
