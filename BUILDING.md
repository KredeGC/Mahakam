# Building Mahakam
Mahakam uses [premake5](https://premake.github.io/download) as build system which will need to be installed and added to ($)PATH.
Generating build-files can be done via the terminal from the project root directory.
The engine currently only supports Windows and Linux operating systems.
There are currently no plans to add Mac support, as I don't have a Mac myself.

## Configurations
Mahakam has 3 different configurations: `Debug`, `DebugOptimized` and `Release`.
Both debug modes have symbols turned on, but the optimized version also has optimizations turned on.
Release mode is the final release version where all optimizations are turned on with minimal debugging support.

## Windows
Run the command below to generate project files.
The project has been tested with VS2019, other versions may or may not work.
```bash
premake5 vs2019
```

This should generate solution and project files for the engine, the editor and a test application sandbox.
The solution file can then simply be opened via Visual Studio or via premake, as mentioned in the [Building using premake](https://github.com/KredeGC/Mahakam/blob/master/BUILDING.md#building-using-premake) section.

## Linux
On linux other libraries are also required to build the engine:
* Xrandr
* Xinerama
* XCursor
* Xi
* Xext

These can usually be installed via the distro's package manager.

In Debian and its derivative distros, these packages can be installed using the command below:
```bash
sudo apt update
sudo apt install -y libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxext-dev
```

Run the command below to generate project files.
```bash
premake5 gmake2
```

Afterwards the project can then be built using Visual Studio Code, where the various options are set up as tasks. It can also be done via the command below:
```bash
make config=[CONFIG]
```
You can also use the command from the [Building using premake](https://github.com/KredeGC/Mahakam/blob/master/BUILDING.md#building-using-premake) section.

## Building using premake
On both windows and linux it is also possible to build using premake, but you won't have as many options available, nor will you be able to build individual projects in the solution.
```bash
premake5 build --config=[CONFIG]
```

## Additional build options
In addition to the `--config` flag, several more build options are available.

### --target
Used during project file generation.
```bash
# This will specify the target platform
# Possible targets are: 'windows' and 'linux'
# Currently only works when cross-compiling from linux to windows using MinGW
--target=[TARGET]
# Example, when generating files on a linux host machine
premake5 gmake2 --target=windows
```

### --toolset
Used during project file generation.
```bash
# This will specify what toolset to use when building
# Possible values are: 'gcc', 'clang' and 'msc'
# Currently msc is only supported on windows, and gcc and clang are only supported on linux
--toolset=[TOOLSET]
# Example, when generating files on linux using the clang compiler
premake5 gmake2 --toolset=clang
```

### --config
Used during project build.
```bash
# This will specify which configuration to build, when using the build command in premake
# Possible values are: 'debug', 'debugoptimized' and 'release'
config=[CONFIG]
# Example, when build a debugoptimized binary with both symbols and optimizations
premake5 build --config=debugoptimized
```

### --unity
Used during project file generation.
```bash
# This will create a unity build of the entire solution
# This option is recommended for full builds of the engine, but may not be practical for incremental changes
--unity
# Example, when generating files on windows
premake5 vs2019 --unity
```

### --standalone
Used during project file generation.
```bash
# *Currently not complete*
# This will generate a binary without profiling or dynamic library linking
# This also strips Editor-related functionality from the engine
# This is intended as the final build process to run as a standalone application
--standalone
# Example, when generating files on windows
premake5 vs2019 --standalone
```