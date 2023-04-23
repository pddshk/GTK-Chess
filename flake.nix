{
  description = "A nix flake for GTK-Chess, UCI chess engine GUI written in C.";
  nixConfig = {
    extra-experimental-features = [ "nix-command" "flakes" ];
    extra-substituters = [ "https://cache.zw3rk.com" ];
    extra-trusted-public-keys = [ "loony-tools:pr9m4BkM/5/eSTZlkQyRt57Jz7OMBxNSUiMC4FkcNfk=" ];

    bash-prompt = "\\[\\e[0m\\][\\[\\e[0;2m\\]nix-develop \\[\\e[0;1m\\]gtk-chess@\\[\\033[33m\\]$(git rev-parse --abbrev-ref HEAD) \\[\\e[0;32m\\]\\w\\[\\e[0m\\]]\\[\\e[0m\\]$ \\[\\e[0m\\]";
  };

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=22.11";
  };

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "x86_64-darwin" ];
      perSystem = nixpkgs.lib.genAttrs supportedSystems;
      allNixpkgs = perSystem (system: import nixpkgs { inherit system; });
      nixpkgsFor = system: allNixpkgs.${system};

      nativeBuildInputs = perSystem (system:
        with nixpkgsFor system; [ clang gnumake pkg-config which libxml2 ]
      );

      buildInputs = perSystem (system:
        with nixpkgsFor system; [ gtk3 librsvg glib ]
      );

      inputsCombined = perSystem (system:
        nativeBuildInputs.${system} ++ buildInputs.${system}
      );
    in
    {
      defaultPackage = perSystem (system: self.packages.${system}.gtkChess);

      packages = perSystem (system:
        with nixpkgsFor system; {
          gtkChess = stdenv.mkDerivation {
            name = "GTK-Chess";
            version = "dev";
            src = ./.;
            nativeBuildInputs = nativeBuildInputs.${system};
            buildInputs = buildInputs.${system};
            buildPhase = "make all";
            installPhase = "mkdir -p $out/bin; cp GTKChess $out/bin";
          };
        }
      );

      devShells = perSystem (system: {
        default =
          with nixpkgsFor system; mkShell {
            buildInputs = [ cppcheck ] ++ inputsCombined.${system};
          };
      });

      formatter = perSystem (system:
        nixpkgs.legacyPackages.${system}.nixpkgs-fmt
      );
    };
}
