# Matrix Code Rain (SDL2 Port)

A high-performance, cross-platform SDL2 C implementation of the iconic *Matrix* digital rain, based on [Rezmason's matrix project](https://github.com/a18project/matrix).

![Matrix Code Rain Preview](https://raw.githubusercontent.com/a18project/matrix/master/screenshot.png)

## ✨ Features

- **Cross-Platform**: Runs natively anywhere SDL2 is supported—Linux, Windows, macOS, Raspberry Pi, retro handheld consoles (Steam Deck, Anbernic, Miyoo, TrimUI), and WebAssembly (Emscripten).
- **Dual Visual Modes**:
  - **Full-Color Phosphor Mode**: Glowing white/mint lead cursor raindrops with radiant fading trails and subtle additive bloom/halo.
  - **Playdate 1-Bit Retro Dither Mode**: Authentic 32-step dither gradient fade using the original Playdate console assets.
- **6 Color Palettes**:
  - 🟢 **Classic Matrix (1999)**: The iconic phosphor green from the original film.
  - 🟩 **Resurrections (2021)**: Modern mint/cyan code styling.
  - 🔴 **Nightmare (Reloaded)**: Crimson red vampire/werewolf program aesthetic.
  - 🟡 **Paradise (Golden)**: Warm amber/gold recreation of the first idyllic Matrix.
  - 🔵 **Twilight (Cyberpunk)**: Electric cyan and neon blue.
  - ⚪ **Terminal (Monochrome)**: Clean, high-contrast monochrome white.
- **Authentic Rain Physics**: Faithful implementation of the original sawtooth wave propagation, dual sinusoidal wobble modulation ($\sqrt{2}$ and $\sqrt{5}$ frequencies), independent column drift, and dynamic glyph cycling.
- **Complete Glyph Set**: Includes all 135 canonical Matrix symbols (cleaned up from official source vectors) plus 10 optional Playdate bonus characters.
- **Self-Contained**: Image loading powered by bundled `stb_image`—no extra `SDL2_image` or external font libraries required.
- **Responsive Grid**: Automatically resizes and scales character columns when resizing the window or toggling fullscreen.

---

## 🎮 Interactive Controls

| Key | Action |
| :--- | :--- |
| `M` | Toggle Mode (**Color** ↔ **Playdate 1-Bit Dither**) |
| `P` / `C` | Cycle Color Palettes (**Classic** → **Resurrections** → **Nightmare** → **Paradise** → **Twilight** → **Terminal**) |
| `Space` | Pause / Resume rain simulation |
| `Up` / `Down` | Increase / Decrease fall speed |
| `[` / `]` | Decrease / Increase raindrop tail length |
| `G` | Toggle cursor glow halo effect |
| `B` | Toggle bonus Playdate glyphs |
| `F` / `F11` | Toggle Fullscreen |
| `F1` | Print command-line help & controls to console |
| `Esc` / `Q` | Quit |

---

## 🛠️ Building & Running

### Requirements
- A C99 compiler (`gcc`, `clang`, or MSVC)
- **SDL2** development library (`libsdl2-dev` on Debian/Ubuntu, `sdl2` via Homebrew, or SDL2 package on Windows)
- *Optional*: `cmake` (version 3.16+)

### Quick Build (Makefile)
```bash
# Ubuntu / Debian
sudo apt install -y libsdl2-dev build-essential

# Build binary
make

# Run
./matrix-sdl
```

### Build with CMake
```bash
mkdir build && cd build
cmake ..
make
./matrix-sdl
```

### Build for macOS (Homebrew)
```bash
brew install sdl2 cmake
mkdir build && cd build
cmake ..
make
./matrix-sdl
```

### Build for Windows (MinGW-w64)
```bash
# In MSYS2 MinGW-w64 shell:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-sdl2 mingw-w64-x86_64-cmake make
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
make
./matrix-sdl.exe
```

### Build for WebAssembly (Emscripten)
```bash
emcmake cmake -B build-web
cmake --build build-web
```

---

## ⚙️ Command-Line Options

```
Usage:
  ./matrix-sdl [options]

Options:
  -m, --mode <color|playdate>    Rendering mode (default: color)
  -p, --palette <name>           Color palette: classic, resurrections, nightmare,
                                 paradise, twilight, terminal
  -s, --speed <float>            Fall speed multiplier (default: 1.0)
  -l, --length <float>           Raindrop tail length (default: 1.15)
  -w, --width <pixels>           Initial window width (default: 1280)
  -h, --height <pixels>          Initial window height (default: 720)
  -f, --fullscreen               Start in fullscreen mode
  -a, --assets <path>            Path to assets directory
  --frames <count>               Run for N frames and exit (headless/automated tests)
  --no-glow                      Disable glow halo around cursors
  --help                         Show help message
```

### Examples
```bash
# Classic 1080p full-color Matrix rain
./matrix-sdl --width 1920 --height 1080

# Playdate 1-bit retro dither mode at 400x240 native resolution
./matrix-sdl --mode playdate --width 400 --height 240

# Nightmare mode with high speed in fullscreen
./matrix-sdl --palette nightmare --speed 1.5 --fullscreen
```

---

## 📁 Project Architecture

```
matrix-sdl/
├── CMakeLists.txt        # Cross-platform CMake configuration
├── Makefile              # Simple direct Makefile
├── README.md             # Project documentation
├── LICENSE               # MIT License
├── assets/
│   ├── matrix-glyphs.png # 20x20 spritesheet (145 authentic glyphs)
│   └── fade-gradient.png # 32-step dither fade gradient
├── include/
│   ├── config.h          # Palette configurations, constants, app settings
│   ├── matrix.h          # Simulation grid and cell data structures
│   ├── renderer.h        # SDL2 rendering pipeline interface
│   └── stb_image.h       # Single-header embedded image decoder
└── src/
    ├── main.c            # SDL2 entry point, event polling, and loop
    ├── matrix.c          # Sawtooth rain simulation, wobble, and cell cycling
    └── renderer.c        # Texture atlas generation, dither mask, and draw calls
```

---

## 📜 Credits & License

- Original Matrix rain algorithm and graphics by [Rezmason](https://github.com/Rezmason) / [a18project](https://github.com/a18project/matrix).
- Glyphs sourced from official Matrix promotional vectors (2007) and *Resurrections* materials.
- Image loading powered by `stb_image` by Sean Barrett.
- Licensed under the [MIT License](LICENSE).
