# Wordup FE

![screenshot](./screenshot.png)

## Description

A conlang word generator

Download the latest from the Releases section on the right

## Usage

Tutorial Video:

![IMAGE ALT](https://img.youtube.com/vi/G9zUSiJPcnM/0.jpg)

There are six tabs:

1. File Menu
   - Close the current file and go back to the open file menu
2. Inventory
   - Displays an interactive IPA table
   - Select the sounds for your language
3. Consonant Categories
   - Create custom groupings of consonants for use in defining syllable structure (∅ and C are automatically created for you)
   - Example: Group "N" for Nasals contains just /m n etc/ from your inventory
4. Onset Maker
   - Select from the consonant groupings to create options for onsets of syllables
5. Coda Maker
   - Select from the consonant groupings to create options for codas of syllables
6. Generate
   - Select the number of syllables to generate
   - Use the data from the previous tabs to generate a new word

## Build

NOTE: You don't need to build! You can download a release!!! Linux users need only install SFML and Windows users can use the release directly!

Dependencies:

- gcc
- make (mingw32-make on Windows)
- OpenGL
- pkg-config (Linux Only)
- [SFML < 3.0](https://www.sfml-dev.org/download/sfml/2.6.1/)
  + Windows users have to download the source bc of the lack of package manager and annoyance of building from source 
    1. Download and extract the root folder (the one with `lib/`, `bin/`, and `include/`) into an "SFML/" folder (i.e. there should be an SFML/include/ folder in the repo root directory)
    2. Copy all the `.dll`s from SFML/bin/ to the root repo directory
    3. Build like normal

Update the submodules: `git submodule update --init --recursive`

Run `make -j<num cpu processors>` (or `mingw32-make` on Windows)

### Via Docker

```
docker build --no-cache -t nonnixos-build nonnixos-build
docker run --rm -v $(pwd):/output nonnixos-build
```
