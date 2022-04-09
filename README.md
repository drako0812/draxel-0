[![GitHub Contributors](https://img.shields.io/github/contributors/drako0812/Pixelly)](https://github.com/drako0812/Pixelly/contributors)
[![GitHub Releases Downloads](https://img.shields.io/github/downloads/drako0812/Pixelly/total)](https://github.com/drako0812/Pixelly/releases)
[![GitHub Stars](https://img.shields.io/github/stars/drako0812/Pixelly)](https://github.com/drako0812/Pixelly/stargazers)
[![GitHub commits since tagged version](https://img.shields.io/github/commits-since/drako0812/Pixelly/0.1.0-pre-alpha/master?include_prereleases)](https://github.com/drako0812/Pixelly/commits/master)
[![License](https://img.shields.io/github/license/drako0812/Pixelly)](LICENSE)

# :video_game: Pixelly

> **NOTE:** *Pixelly* is a WIP name for this project. I would like to come up with a catchier name in the near future. I also have not investigated
> whether or not the name is used for anything else, yet.

*Pixelly* is a [Fantasy Console/Computer](https://en.wikipedia.org/wiki/Fantasy_video_game_console) which has been developed in modern C++ (up to and including C++20). It currently uses [SFML2](https://github.com/SFML/SFML) for its backend, and [Lua](https://lua.org) via [Sol](https://sol2.rtfd.io) for scripting. Although, I'd like to support both more backends and more scripting languages in the future.

## :memo: Specifications

### :art: Graphics

*Pixelly* has a 384x216 pixel screen resolution.

Currently, it only has a 48x27 text display mode, but pixel display modes are definitely planned.

It has a 16-color palette that will be modifiable (but not currently).

### :keyboard: Input

Currently, *Pixelly* can only accept keyboard input, but support for mouse and gamepad input is on the todo list.

## :books: Documentation

You can find detailed documentation for *Pixelly* on the [Wiki](https://github.com/drako0812/Pixelly/wiki).

## :hammer: Building

I've spent quite a while trying to make sure that *Pixelly* is as simple to build as possible, but as additional features are added it's possible it may become more difficult.

This should be how to build *Pixelly* on at least **Windows** and **Linux**:

- Make sure you have a modern C++ compiler installed (ie. MSVC, GCC, or Clang)
- Install [vcpkg](https://github.com/Microsoft/vcpkg)
- Install [CMake](https://cmake.org)
- Install [git](https://git-scm.com)
- Build *Pixelly*:
  ```bat
  git clone https://github.com/drako0812/Pixelly
  cd Pixelly
  git submodule init
  git submodule update
  mkdir build
  cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
  cmake --build build
  ```
  You might also want to pass `-DCMAKE_BUILD_TYPE=Release` to `cmake` to build in *Release* mode.
- Now you should be able to run *Pixelly*
  ```bat
  cd build

  ./Pixelly.exe
  --OR--
  ./Pixelly
  ```
- After building you should be able to move the contents of the `build` directory to wherever you want.
