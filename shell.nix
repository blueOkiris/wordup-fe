{ pkgs ? import <nixpkgs> { } }:

let
    buildLibs = with pkgs; (with xorg; [
        gtk4
    ]);
in with pkgs; mkShell {
    buildInputs = [
        gcc
        gnumake
        ccls
        pkg-config
    ] ++ buildLibs;
    shellHook = ''
        export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:${lib.makeLibraryPath buildLibs}"
    '';
}

