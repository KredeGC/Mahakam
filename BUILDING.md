# Building Mahakam
Mahakam uses premake5 as build system.
Generating build-files can be done via the terminal from the project root directory.
The engine currently only supports Windows and Linux operating systems.
There are no plans to add Mac support, as I don't have a Mac myself.

## Configurations
Mahakam has 3 different configurations: Debug, DebugOptimized and Release.
Both debug modes have symbols turned on, but the optimized version also has optimizations turned on.
Release mode is the final release version where all optimizations are turned on with minimal debugging support.

## Windows
Run the command below to generate project files.
The project has been tested with VS2019, other versions may or may not work.
```bash
premake5.exe vs2019
```

This should generate solution and project files for the engine, the editor and a test application sandbox.
The solution file can then simply be opened via Visual Studio.

## Linux
In addition to premake, other libraries are also needed, including X11, Xrandr.
These can usually be installed via the distro's package manager.
Run the command below to generate project files.
```bash
premake5 gmake2
```

Afterwards the project can then be built using the command below, or using Visual Studio Code, where the various options are set up as tasks.
```bash
make [PROJECT] config=[CONFIG]
```