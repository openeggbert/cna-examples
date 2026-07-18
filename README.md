# CNA Examples

A single, cross-platform, in-app catalog of live demonstrations for every area of
[CNA](https://github.com/openeggbert/cna) — a C++ reimplementation of the XNA 4.0 programming
model built on SDL3. Conceptually inspired by
[javafx-ensemble8](https://github.com/lusalome/javafx-ensemble8) (browsable sample catalog with
in-app navigation), but entirely CNA-specific — no JavaFX code, assets, or dependency involved.

Pick an area from the home menu (Input, Audio, Devices, Net, Media, 2D Graphics, 3D Graphics, ...),
drill into a category, and run the demo — all inside one application, on desktop, web or mobile.

See [plan.md](plan.md) for the full architecture and navigation design.

## Status

Early skeleton: the app boots, shows the Home menu, and navigates Area → Category, but
individual demo screens have not been implemented yet (see [plan.md](plan.md) §8, "Scope of
this pass"). Only the `EASYGL` graphics backend is targeted for now.

## Platforms

| Tier | Platforms |
|---|---|
| Now | Windows, Linux, Web (Emscripten), Android |
| Future | macOS, iPhone, consoles |

## Prerequisites

| Tool | Version |
|---|---|
| CMake | ≥ 3.20 |
| C++ compiler | C++23 (GCC 13+, Clang 16+, MSVC 19.38+) |
| CNA | sibling directory `../cna` |
| sharp-runtime | sibling directory `../sharp-runtime` |

Clone all three side-by-side:

```
openeggbert/
├── cna/
├── sharp-runtime/
└── cna-examples/       ← this repo
```

## Building

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Run it:

```bash
./build/cna_examples
```

The graphics backend defaults to `EASYGL`; override with
`-DCNA_GRAPHICS_BACKEND=<SDL_RENDERER|EASYGL|VULKAN|BGFX|WEBGPU>` if needed (subject to CNA's
own backend maturity — see CNA's `CLAUDE.md`).

## License

MIT — see [LICENSE](LICENSE). This is original CNA-specific work, not a port of Microsoft's
XNA sample collection (compare `../cna-samples`, which is Ms-PL for that reason).
