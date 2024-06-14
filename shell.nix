{ pkgs ? import <nixpkgs> { } }:

let
    buildLibs = with pkgs; (with xorg; [
        sfml
        libGL
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

