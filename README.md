# Pathfinder

Interactive pathfinding visualizer built with C++20 and SDL2. Visualizes BFS, DFS, Dijkstra and A* on a grid with terrain and wall editing.

## Table of Contents
- [Quick Start](#quick-start)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Build](#build)
- [Compiler flags and C++20](#compiler-flags-and-c20)
- [Usage](#usage)
	- [Controls](#controls)
- [Assets](#assets)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## Quick Start
1. Install the development dependencies for your OS (see **Installation**).
2. From the project root run `make`.
3. Launch the app with `./compile` or `make run`.

## Features
- Interactive grid with UI buttons for selecting algorithm, terrain and controls
- Implements: BFS, DFS, Dijkstra, A* (visualized step-by-step)
- Supports multiple terrain types with weighted costs (grass, mud, water)
- Speed control and keyboard(not fully functional yet) shortcuts for quick experimentation

## Requirements
- A C++ compiler with C++20 support (see notes below)
- SDL2 development libraries (`sdl2-config` must be available)
- SDL2_ttf development library (for TTF font rendering)
- `make` and standard build tools

## Installation
### Debian / Ubuntu
```bash
sudo apt update
sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev
```

### Fedora
```bash
sudo dnf install gcc-c++ SDL2-devel SDL2_ttf-devel
```

### macOS (Homebrew)
```bash
brew install sdl2 sdl2_ttf
```

If you need image support (optional), install the SDL2_image development package and uncomment `-lSDL2_image` in the Makefile.

## Build
The repository includes a simple `make` workflow. From the project directory:

```bash
make
# then
./compile
# or
make run
```

The Makefile compiles all `.cpp` files and links with the output of `sdl2-config` and `-lSDL2_ttf`.

Manual build example (single command):

```bash
g++ -std=c++20 -O2 -Wall *.cpp $(sdl2-config --cflags --libs) -lSDL2_ttf -o compile
```

If you prefer `clang++`, replace `g++` with `clang++` and ensure your standard library supports C++20 features used in this project.

## Compiler flags and C++20
This project targets C++20. The Makefile currently sets:

```makefile
CXX_FLAGS = -std=c++20 -Wall
```

To enable optimizations or extra warnings, edit the `CXX_FLAGS` line in the `makefile`, for example:

```makefile
CXX_FLAGS = -std=c++20 -O2 -Wall -Wextra -march=native
```

Notes about `std::format` and C++20 library support:
- The code uses `<format>` (`std::format`) and other C++20 facilities. Use a recent toolchain with full C++20 support (GCC 11+/12+ or Clang 12+/14+ recommended).
- If your system's standard library doesn't yet implement `<format>`, either upgrade your toolchain or replace formatting calls with alternatives (e.g. `std::ostringstream` or the `fmt` library).

## Usage
Run the compiled binary (`./compile`) and interact with the window. Typical workflow:

1. Use the UI buttons or the keyboard(not fully functional yet) to set the source and target cells.
2. Draw walls / terrain using the drawing tools.
3. Choose an algorithm (BFS / DFS / DIJKSTRA / ASTAR).
4. Press the `RUN` button or press `Space` to start the visualization.

### Controls
- Mouse left-click UI buttons to change mode, terrain and algorithm.
- In the drawing area:
    - left-click to set source/target or draw walls/terrain depending on mode.
    - right-click to erase the wall/terrain
- Keyboard shortcuts:
	- `1` — set source mode
	- `2` — set target mode
	- `3` — draw wall mode
	- `Space` — start search (requires source and target)
	- `C` — clear search
	- `R` — reset grid
	- `-` / `+` — decrease / increase visualization speed

## Assets
The app expects the font file `BebasNeue-Regular.ttf` to be available in the working directory. If you don't have this font, either:
- place a TTF file named `BebasNeue-Regular.ttf` in the project root, or
- edit `app.cpp` to point to a different font path.

## Troubleshooting
- `sdl2-config` not found: install the `libsdl2-dev` package (Debian/Ubuntu) or the equivalent for your OS.
- `TTF_OpenFont` errors: ensure the TTF font exists at the path used by `app.cpp`.
- Compilation errors related to `<format>`: upgrade your compiler or replace `std::format` uses.

## Contributing
Contributions and issues are welcome. Please open a pull request or an issue with a description and steps to reproduce. Keep changes C++20-compatible.

## License
This project is licensed under the GNU General Public License v3.0. See [LICENSE](LICENSE) for details.
