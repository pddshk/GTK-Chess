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

  outputs = { self, nixpkgs }: {
    pkgs = import nixpkgs { system = "x86_64-linux"; };

    nativeBuildInputs = with self.pkgs; [ clang gnumake pkg-config which libxml2 ];
    buildInputs = with self.pkgs; [ gtk3 librsvg glib ];

    defaultPackage.x86_64-linux = self.packages.gtkChess;

    packages.gtkChess = with self.pkgs; stdenv.mkDerivation {
      name = "GTK-Chess";
      version = "dev";
      src = ./.;
      nativeBuildInputs = self.nativeBuildInputs;
      buildInputs = self.buildInputs;
      buildPhase = "make all";
      installPhase = "mkdir -p $out/bin; cp GTKChess $out/bin";
    };

    devShells.x86_64-linux.default = with self.pkgs; mkShell { 
      buildInputs = [ cppcheck ] ++ self.nativeBuildInputs ++ self.buildInputs; 
    };
  };

}
