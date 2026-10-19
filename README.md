[![Linux build](https://github.com/VoxelStorm-Ltd/fractyr/actions/workflows/build-linux.yml/badge.svg)](https://github.com/VoxelStorm-Ltd/fractyr/actions/workflows/build-linux.yml)
[![Windows build](https://github.com/VoxelStorm-Ltd/fractyr/actions/workflows/build-win.yml/badge.svg)](https://github.com/VoxelStorm-Ltd/fractyr/actions/workflows/build-win.yml)
[![macOS build](https://github.com/VoxelStorm-Ltd/fractyr/actions/workflows/build-mac.yml/badge.svg)](https://github.com/VoxelStorm-Ltd/fractyr/actions/workflows/build-mac.yml)

# Fractyr

A vaguely Descent-inspired abstract 6-degree-of-freedom indoor flight sim / tunnel shooter.

3D engine and all gameplay written from scratch in five days by a two-man team.

The world is procedurally generated at runtime using 3D Voronoi cells, dynamically triangulated, mapping the interior of a Mandelbulb 3D fractal.

## Downloading

Automated builds produce binaries downloadable from the [releases](https://github.com/VoxelStorm-Ltd/fractyr/releases) page.

Original releases available to download at https://fractyr.com or https://voxelstorm.itch.io/fractyr.

## Controls
Best played with a joystick.

- WSAD, Space & ctrl or joystick throttle and hat switch to fly the ship
- Mouse, joystick or arrow keys to steer
- Mouse buttons or joystick buttons to fire weapons
- F11: toggle between full-screen and windowed view
- Pause: pause the game.
- Escape: quit

## Building and running

Run the commands from the repository root. CMake defaults to `Release`, but specifying the build mode explicitly and using separate build directories makes switching between `Debug` and `Release` straightforward.

For a 64-bit Linux Release build:

```sh
cmake -S . -B build/linux64-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_32BIT=OFF
cmake --build build/linux64-release --parallel
./build/linux64-release/Fractyr
```

For a 64-bit Linux Debug build:

```sh
cmake -S . -B build/linux64-debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_32BIT=OFF
cmake --build build/linux64-debug --parallel
./build/linux64-debug/Fractyr
```

For a 32-bit Windows Release build cross-compiled with MinGW on Linux:

```sh
cmake -S . -B build/win32-release \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_32BIT=ON
cmake --build build/win32-release --parallel
wine build/win32-release/Fractyr.exe
```

For its Debug equivalent, change `CMAKE_BUILD_TYPE` and use a separate directory:

```sh
cmake -S . -B build/win32-debug \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
  -DCMAKE_BUILD_TYPE=Debug \
  -DBUILD_32BIT=ON
cmake --build build/win32-debug --parallel
wine build/win32-debug/Fractyr.exe
```

For a native Apple Silicon macOS Release build:

```sh
brew install boost ftgl freetype glew glfw libpng libvorbis portaudio voro++
cmake -S . -B build/mac-arm64-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
  -DCMAKE_PREFIX_PATH="$(brew --prefix)" \
  -DFRACTYR_BREW_PREFIX="$(brew --prefix)" \
  -DBUILD_32BIT=OFF
cmake --build build/mac-arm64-release --parallel
./build/mac-arm64-release/Fractyr
```

`CMAKE_BUILD_TYPE` accepts `Debug` or `Release`. Debug builds use `-Og` with debugging information, enable general logging, and suppress the especially verbose input and sound debug streams. Linux64 Debug builds additionally enable AddressSanitizer and UndefinedBehaviorSanitizer. Release builds disable standard and Boost assertions; the bundled x86 targets use the original optimized, LTO-enabled configuration.

`BUILD_32BIT` defaults to the compiler's architecture. On x86, `ON` selects `-m32` and `lib/<platform>32`; `OFF` selects `-m64` and `lib/<platform>64`. On Apple Silicon, `OFF` selects native arm64 and Homebrew libraries rather than the bundled Intel-only `lib/mac64` archives. Oculus Rift support is disabled on Apple Silicon because the repository only includes an Intel Oculus SDK library. The repository currently contains bundled library sets for Win32, Linux64, and Intel Mac64; Linux32 and Win64 cannot link until corresponding `lib/linux32` and `lib/win64` dependencies are supplied.

All builds require CMake, a matching C++ toolchain, Bash, `xxd`, and Boost headers. Shader, font, and music resources are compiled into the executable automatically. Linux, Windows, and Intel macOS use the bundled GLEW library; Apple Silicon uses the Homebrew GLEW library. MinGW cross-builds automatically select the resource-embedding mode compatible with the Linux build host.
