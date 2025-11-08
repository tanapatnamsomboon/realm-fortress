# Realm Fortress

A cross-platform 3D medieval city-building simulation written in **C++23**, using **OpenGL**, **GLFW**, and **CMake**.

## Features
- Cross-platform (Windows, Linux, macOS)
- 3D rendering with OpenGL + GLAD
- Scene, Renderer, Model loading (via Assimp)
- UI with Dear ImGui
- Modular code structure for expansion

## Build Instructions
```bash
git clone https://github.com/tanapatnamsomboon/medieval-colony.git
cd medieval-colony
cmake -B build
cmake --build build
./build/MedievalColony
```

## Dependencies
- GLFW
- GLAD
- GLM
- stb
- Assimp
- ImGui

## Theme

Medieval city-building on hexagonal terrain, inspired by Banished, Timberborn, and RimWorld.

## License

[MIT License](./LICENSE)
