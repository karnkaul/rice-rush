# Rice Rush

**Simple 2D game using vulkify and capo**

## Installation

### Supported Platforms

1. x64 Windows 10
1. x64 Linux: X11, Wayland (untested)

Grab the [latest release](https://github.com/karnkaul/rice-rush/releases) and unzip to a desired location, or build and _install_ (TODO) via CMake.

## Controls

#### Keyboard

1. WASD / cursor keys to move
1. Space to interact
1. Enter to restart

#### Gamepad

1. Left stick to move
1. A to interact
1. Start to restart

## Modding Data

All the resources used by the game are fully customizable, with sample assets in `data/`. `manifest.txt` contains the mapping for each in-game resource to an on-disk asset.

## Building

### Requirements

1. CMake 3.18+
1. Desktop operating system
  1. Windows 10
  1. Linux: X11, Wayland (untested)
  1. MacOSX (experimental, requires MoltenVk)

### Steps

If you'd like to use CMake presets shipped with `vulkify`, `cmake/CMakePresets.json` can be symlinked / copied to the root directory.

1. IDEs / CMake Tools
  1. Generate corresponding IDE project through CMake if required
  1. Open root directory / generated project in IDE
  1. Select a kit / preset and configure it if required
  1. Build project / `ALL_BUILD`
  1. Debug / run the `rice-rush` target / executable
1. CLI
  1. Configure a build via `cmake -S . -B out/build`
    1. Use a preset via `--preset=ninja-debug`; list the ones available via `--list-presets`
  1. Build via `cmake --build out/build`
  1. Run via `out/build/rice-rush[.exe]`
