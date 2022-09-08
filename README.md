<div align="center">
<img width=218 height=218 alt="Mahakam" src="/.github/Mahakam.png?raw=true" />
<h1>Mahakam</h1>

![Windows supported](https://img.shields.io/badge/Windows-win--64-green?style=flat-square)
![Linux supported](https://img.shields.io/badge/Linux-Ubuntu-green?style=flat-square)
![MacOS not supported](https://img.shields.io/badge/MacOS-Not%20Supported-red?style=flat-square)
![Android not supported](https://img.shields.io/badge/Android-Not%20Supported-red?style=flat-square)
![IOS not supported](https://img.shields.io/badge/IOS-Not%20Supported-red?style=flat-square)

<p>A somewhat functional game engine with suppport for rendering and sound (currently).
More functionality will be added as I find the will to work on it.
Physics will be handled by Bullet3 once integrated fully.</p>

[![License](https://img.shields.io/github/license/KredeGC/Mahakam?style=flat-square)](https://github.com/KredeGC/Mahakam/blob/master/LICENSE)
[![Issues](https://img.shields.io/github/issues/KredeGC/Mahakam?style=flat-square)](https://github.com/KredeGC/Mahakam/issues)
[![Build](https://img.shields.io/github/workflow/status/KredeGC/Mahakam/Build?style=flat-square)](https://github.com/KredeGC/Mahakam/actions/workflows/main.yml)

</div>

# Libraries and technologies
* Uses OpenGL for rendering. No other rendering APIs are supported currently, but may in the future.
* [Bullet Physics SDK](https://github.com/bulletphysics/bullet3) for physics and collision detection.
* [EnTT](https://github.com/skypjack/entt) as entity-component-system.
* [glad](https://github.com/Dav1dde/glad) for interfacing with OpenGL.
* [GLFW](https://github.com/glfw/glfw) for window creation and management.
* [OpenGL Mathematics](https://github.com/g-truc/glm) for fast SIMD vector math.
* [Dear ImGui](https://github.com/ocornut/imgui) for GUI interaction and rendering.
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) for editor interaction and rendering.
* [miniaudio](https://github.com/mackron/miniaudio) for audio playback.
* [spdlog](https://github.com/gabime/spdlog) for easy and fast logging.
* [stb_image](https://github.com/nothings/stb) for loading various image files.
* [Steam Audio](https://valvesoftware.github.io/steam-audio/) for audio spatialization.
* [tinygltf](https://github.com/syoyo/tinygltf/tree/skinning) for model and animation loading.
* [yaml-cpp](https://github.com/jbeder/yaml-cpp) for serializaing and deserializing various YAML files.

# Usage
Mahakam can be used by itself, simply by building and linking to it.
An editor application also exists, called Erebor, which includes a level-editor and asset manager among other game-making tools.
The best way to understand and use the engine is to build and run it, and then change around stuff to see how it affects the state.

# Erebor
Running the editor can be done via Visual Studio or Visual Studio Code, via their respective debuggers.
It can also be run using various files within the Erebor directory, namely: EreborDebug, EreborOptimized and EreborRelease.
There are both bash and batch files for Linux and Windows.

# Building
Refer to the [BUILDING.md](BUILDING.md) file.

# Media
![Editor 1](https://github.com/KredeGC/Mahakam/blob/master/.github/media/editor1.png)
![Editor 2](https://github.com/KredeGC/Mahakam/blob/master/.github/media/editor2.png)