{
  description = "Mars Lander / C++ Concurrency";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      packages.${system}.default = pkgs.stdenv.mkDerivation {
        pname = "mars-lander";
        version = "0.1.0";

        src = ./.;

        nativeBuildInputs = with pkgs; [
          cmake
          ninja
        ];

        cmakeGenerator = "Ninja";
      };

      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          gcc
          cmake
          ninja
          clang-tools
          cmake-format

          python3
          python3Packages.jupyterlab
          python3Packages.numpy
          python3Packages.matplotlib
          python3Packages.pandas
          python3Packages.seaborn
          python3Packages.ruff
        ];

        shellHook = ''
          echo "Entered dev environment"
        '';
      };

      formatter.${system} = pkgs.nixfmt;
    };
}
