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
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          gcc
          gnumake
          clang-tools
          python3
          python3Packages.jupyterlab
          python3Packages.numpy
          python3Packages.matplotlib
          python3Packages.ruff
        ];

        shellHook = ''
          echo "Entered dev environment"
        '';
      };
      formatter.${system} = pkgs.nixfmt;
    };
}
