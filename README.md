# 🔥 FireEmblem

> A Fire Emblem-inspired game built from scratch using SDL3 in C++17.

![C++](https://img.shields.io/badge/C++-17-blue?logo=cplusplus)
![SDL3](https://img.shields.io/badge/SDL-3-green)
![License](https://img.shields.io/badge/license-MIT-brightgreen)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey)

---

## ✨ Features

- 🗺️ **Tilemap System** — TMX/TMJ map loading via tmxlite, multi-layer rendering with Y-Sort
- 🎬 **Animation System** — Keyframe-based animation with Linear & Discrete tracks
- 🛠️ **Editor UI** — In-game inspector & hierarchy panels powered by Dear ImGui (Docking)
- 📦 **Asset Manager** — UUID-based asset loading with embedded XML support
- 🧩 **Component System** — Entity-component architecture

---

## 🔧 Dependencies

| Library | Source |
|--------|--------|
| SDL3 | vcpkg |
| SDL3_image | vcpkg |
| SDL3_ttf | vcpkg |
| spdlog | vcpkg |
| nlohmann_json | vcpkg |
| tinyxml2 | vcpkg |
| [Dear ImGui](https://github.com/ocornut/imgui) | submodule (docking branch) |

---

## 🚀 Getting Started

### Prerequisites

- CMake 3.25+
- MinGW-w64 (x64)
- [vcpkg](https://github.com/microsoft/vcpkg)

### Clone

```bash
git clone --recurse-submodules https://github.com/HuaDong1120/FireEmblem.git
cd FireEmblem
```

### Configure & Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

> Make sure your `CMAKE_TOOLCHAIN_FILE` points to your local vcpkg installation.

---

## 📁 Project Structure

```
FireEmblem/
├── src/          # Source files
├── include/      # Header files
├── assets/       # Game assets (maps, textures, audio)
├── 3rdparty/
│   └── imgui/    # Dear ImGui (submodule)
└── CMakeLists.txt
```

---

## 📸 Screenshot

> Coming soon...

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
