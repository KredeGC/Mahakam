# Mahakam
A functional game engine with suppport for rendering and sound (currently).
More functionality will be added as I find the will to work on it.
Physics will be handled by Bullet3 once integrated fully.

# Libraries and technologies
* Uses OpenGL for rendering. No other rendering APIs are supported currently, but may in the future.
* [assimp](https://github.com/assimp/assimp) for model and animation loading.
* [Bullet Physics SDK](https://github.com/bulletphysics/bullet3) for physics and collision detection.
* [EnTT](https://github.com/skypjack/entt) as entity-component-system.
* [glad](https://github.com/Dav1dde/glad) for interfacing with OpenGL.
* [GLFW](https://github.com/glfw/glfw) for window creation and management.
* [OpenGL Mathematics](https://github.com/g-truc/glm) for fast SIMD vector math.
* [Dear ImGui](https://github.com/ocornut/imgui) for GUI interaction and rendering.
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) for editor interaction and rendering.
* [miniaudio](https://github.com/mackron/miniaudio) for audio playback.
* [robin_hood](https://github.com/martinus/robin-hood-hashing) for fast unordered sets and maps.
* [spdlog](https://github.com/gabime/spdlog) for easy and fast logging.
* [stb_image](https://github.com/nothings/stb) for loading various image files.
* [Steam Audio](https://valvesoftware.github.io/steam-audio/) for audio spatialization.
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
Refer to the BUILDING.md file.

# Media