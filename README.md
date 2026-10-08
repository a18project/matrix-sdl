# Matrix Code Rain (SDL2 Port)

A high-performance, cross-platform SDL2 C implementation of the iconic *Matrix* digital rain, based on [Rezmason's matrix project](https://github.com/a18project/matrix).

![Matrix Code Rain Preview](https://raw.githubusercontent.com/a18project/matrix/master/screenshot.png)

## ✨ Features

- **In-Game Settings GUI Overlay**: Interactive cyberpunk settings menu accessible via `Tab`, `F2`, on-screen gear button, or Gamepad `SELECT`/`MENU`. Features 4 tabs: **Versions**, **Effects**, **Tuning**, and **About**.
- **12 Matrix Versions & Modes**:
  - 🟢 **Classic (1999)**: The iconic phosphor green digital rain as seen in the trilogy.
  - 🟩 **Resurrections (2021)**: Modern Matrix Resurrections code with crisp mint accents.
  - 💻 **Operator Terminal (1999)**: Rapid, flat, dense 1999 operator monitor with rapid streams.
  - 🔴 **Nightmare (Reloaded)**: Merovingian's gothic vampire code: foreboding crimson, high speed & slant.
  - 🟡 **Paradise (Gnostic)**: The first idyllic predecessor Matrix: hypnotic golden amber, slow rain.
  - 🩵 **Palimpsest (Furious Angels)**: Rob Dougan inspired: teal & golden amber contrast with reverse slant.
  - 🟣 **Twilight (Cyberpunk)**: Vibrant futuristic palette with cyan, neon magenta & indigo.
  - 🔮 **Morpheus (Zion)**: Deep royal purple and crimson tones honoring Morpheus.
  - 👑 **Trinity (Awakened)**: Emerald matrix streams illuminated by radiant gold glints.
  - 🔵 **Bugs (Blue Pill)**: Electric cerulean & cyan code styling inspired by Bugs.
  - 🏙️ **Megacity (Revolutions)**: Revolutions opening titles variation with slower descent.
  - 🕹️ **Playdate (1-Bit Retro)**: Authentic 32-step dithered black-and-white handheld render.
- **Special Post-Processing & Stripe Effects**:
  - **Standard Palette**: Tone-mapped color gradients.
  - **Rainbow Pride Stripes**: 6-color rainbow pride flag vertical bands across columns.
  - **Trans Pride Stripes**: 5-color trans pride flag vertical bands across columns.
  - **Cyberpunk Dual Stripes**: Alternating neon cyan & magenta vertical stripes.
  - **Monochrome Terminal**: High-contrast pure white on black phosphor.
  - **Vintage Amber CRT**: Classic monochrome amber monitor aesthetic.
- **Cross-Platform**: Runs natively anywhere SDL2 is supported—Linux, Windows, macOS, Raspberry Pi, retro handheld consoles (Miyoo Flip, Steam Deck, Anbernic, Miyoo, TrimUI), and WebAssembly (Emscripten).
- **Gamepad & Handheld Navigation**: Full support for D-Pad, face buttons, shoulder triggers, and mouse controls.
- **Authentic Rain Physics**: Faithful implementation of sawtooth wave propagation, dual sinusoidal wobble modulation ($\sqrt{2}$ and $\sqrt{5}$ frequencies), independent column drift, slant angle, and dynamic glyph cycling.
- **Self-Contained**: Image loading powered by bundled `stb_image` and UI powered by an embedded 8x8 font table—zero extra dependencies beyond base `libsdl2`.

---

## 🎮 Interactive Controls

### Keyboard & Mouse
| Key | Action |
| :--- | :--- |
| `Tab` / `F2` | **Toggle Settings & Effects GUI Menu** |
| `Arrows` / `Mouse` | Navigate menu options, click to select / drag sliders |
| `M` | Quick toggle Mode (**Color** ↔ **Playdate 1-Bit Dither**) |
| `P` / `C` | Quick cycle Color Palettes |
| `Space` | Pause / Resume rain simulation |
| `Up` / `Down` | Increase / Decrease fall speed |
| `[` / `]` | Decrease / Increase raindrop tail length |
| `G` | Toggle cursor glow halo effect |
| `B` | Toggle bonus Playdate glyphs |
| `F` / `F11` | Toggle Fullscreen |
| `F1` | Print command-line help & controls to console |
| `Esc` / `Q` | Close Settings GUI / Quit application |

### Handheld Gamepad (Miyoo Flip / PortMaster)
| Button | Action |
| :--- | :--- |
| `SELECT` / `MENU` | **Toggle Settings & Effects GUI Menu** |
| `D-Pad Up / Down` | Navigate menu items / adjust speed |
| `D-Pad Left / Right` | Adjust sliders & cycle values / adjust tail length |
| `L1` / `R1` | Switch menu tabs |
| `Button A` | Select / Activate option / Toggle mode |
| `Button B` | Go back / Close Settings Menu |
| `Button X` | Toggle glow halo |
| `Button Y` | Toggle bonus glyphs |
| `START` | Pause / Resume simulation |

---

## ⚙️ Command-Line Options

```
Usage:
  ./matrix-sdl [options]

Options:
  -v, --version <name>           Matrix version: classic, resurrections, operator,
                                 nightmare, paradise, palimpsest, twilight,
                                 morpheus, trinity, bugs, megacity, playdate
  -e, --effect <name>            Effect: palette, pride, trans, stripes
  -m, --mode <color|playdate>    Rendering mode (default: color)
  -p, --palette <name>           Color palette: classic, resurrections, operator,
                                 nightmare, paradise, palimpsest, twilight,
                                 morpheus, trinity, bugs, terminal, amber
  -s, --speed <float>            Fall speed multiplier (default: 1.0)
  -l, --length <float>           Raindrop tail length (default: 1.15)
  --slant <float>                Rain slant angle (-0.5 to 0.5)
  -w, --width <pixels>           Initial window width (default: 1280)
  -h, --height <pixels>          Initial window height (default: 720)
  -f, --fullscreen               Start in fullscreen mode
  -a, --assets <path>            Path to assets directory
  --frames <count>               Run for N frames and exit (headless/automated tests)
  --no-glow                      Disable glow halo around cursors
  --help                         Show help message
```

---

## 🛠️ Building & Running

### Requirements
- A C99 compiler (`gcc`, `clang`, or MSVC)
- **SDL2** development library (`libsdl2-dev` on Debian/Ubuntu, `sdl2` via Homebrew, or SDL2 package on Windows)

### Quick Build (Makefile)
```bash
make
./matrix-sdl
```

### Build with CMake
```bash
mkdir build && cd build
cmake ..
make
./matrix-sdl
```

---

## 📜 Credits & License

- Original Matrix rain algorithm and graphics by [Rezmason](https://github.com/Rezmason) / [a18project](https://github.com/a18project/matrix).
- Glyphs sourced from official Matrix promotional vectors (2007) and *Resurrections* materials.
- Image loading powered by `stb_image` by Sean Barrett.
- Licensed under the [MIT License](LICENSE).
