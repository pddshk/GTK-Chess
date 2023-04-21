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
    defaultPackage.x86_64-linux = self.packages.gtkChess;

    packages.gtkChess = 
      with import nixpkgs { system = "x86_64-linux"; }; 
      stdenv.mkDerivation {
        name = "GTK-Chess";
        version = "dev";
        src = ./.;
        nativeBuildInputs = [ clang gnumake pkg-config which libxml2 ];
        buildInputs = [ gtk3 librsvg glib ];
        buildPhase = "make all";
        installPhase = "mkdir -p $out/bin; cp GTKChess $out/bin";
      };
  };
}
