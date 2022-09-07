newoption {
    trigger = "toolset",
    value = "Toolset (eg. gcc, clang, msc)",
    description = "The toolset to use to compile with",
    default = "gcc",
    allowed = {
        { "gcc", "gcc and g++ using ld and ar" },
        { "clang", "clang and clang++ using lld and llvm-ar" },
        { "msc", "msbuild and cl using link.exe" }
    }
}

newoption {
    trigger = "target",
    value = "Platform",
    description = "The target platform to compile to",
    default = os.host(),
    allowed = {
        { "windows", "Windows system" },
        { "linux", "Unix-based systems" }
    }
}

newoption {
    trigger = "config",
    value = "Configuration",
    description = "The configuration to compile",
    default = "debug",
    allowed = {
        { "debug", "Debug build with symbols turned on" },
        { "debugoptimized", "Debug build with symbols and optimizations turned on" },
        { "release", "Release build without symbols, but with optimizations" }
    }
}

newaction {
    trigger = "build",
    description = "Builds the project using whatever toolset is chosen",
    onProject = function(prj)
        printf("Building project '%s'", prj.name)
        
        if (os.host() == "windows") then
            os.execute("msbuild "..prj.location.."\\"..prj.name..".vcxproj -t:Build -verbosity:minimal -p:Configuration=".._OPTIONS["config"].." -p:Platform=x64")
        elseif (os.host() == "linux") then
            os.execute("make -j3 "..prj.name.." config=".._OPTIONS["config"])
        end
    end
}

outputdir = "%{cfg.buildcfg}-%{_OPTIONS['target']}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["bullet"]            = "../Mahakam/vendor/bullet/src"
IncludeDir["bullet_dynamics"]   = "../Mahakam/vendor/bullet/src/BulletDynamics"
IncludeDir["bullet_collision"]  = "../Mahakam/vendor/bullet/src/BulletCollision"
IncludeDir["linear_math"]       = "../Mahakam/vendor/bullet/src/LinearMath"
IncludeDir["entt"]              = "../Mahakam/vendor/entt/include"
IncludeDir["GLFW"]              = "../Mahakam/vendor/GLFW/include"
IncludeDir["glad"]              = "../Mahakam/vendor/glad/include"
IncludeDir["glm"]               = "../Mahakam/vendor/glm"
IncludeDir["imgui"]             = "../Mahakam/vendor"
IncludeDir["imguizmo"]          = "../Mahakam/vendor"
IncludeDir["miniaudio"]         = "../Mahakam/vendor/miniaudio/include"
IncludeDir["robin_hood"]        = "../Mahakam/vendor"
IncludeDir["spdlog"]            = "../Mahakam/vendor/spdlog/include"
IncludeDir["stb_image"]         = "../Mahakam/vendor/stb_image"
IncludeDir["steamaudio"]        = "../Mahakam/vendor/steamaudio/include"
IncludeDir["tiny_gltf"]         = "../Mahakam/vendor/tiny_gltf"
IncludeDir["yaml"]              = "../Mahakam/vendor/yaml-cpp/include"


SteamAudioLibDir = "../Mahakam/vendor/steamaudio/lib/".._OPTIONS["target"].."-x64"

VendorIncludes = {
    "src",
    "../Mahakam/src",
    "%{IncludeDir.entt}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.imgui}",
    "%{IncludeDir.imguizmo}",
    "%{IncludeDir.robin_hood}",
    "%{IncludeDir.spdlog}",
    "%{IncludeDir.yaml}"
}

VendorLibDirs = {
    SteamAudioLibDir
}

VendorLinks = {
    "Mahakam",
    "BulletDynamics",
    "BulletCollision",
    "LinearMath",
    "GLFW",
    "glad",
    "ImGui",
    "ImGuizmo",
    "yaml-cpp",
    "phonon"
}

-- MinGW requires some extra links
if (os.host() == linux and _OPTIONS["target"] == "windows") then
    MinGWLinks = {
        "gdi32",
        "dwmapi",
        "comdlg32"
    }
end

-- X11 and unix-specific stuff
LinuxLinks = {
    "Xrandr",
    "Xi",
    --"GLU",
    --"GL",
    "X11",
    "dl",
    "pthread",
    "stdc++fs"	--GCC versions 5.3 through 8.x need stdc++fs for std::filesystem
}

workspace "Mahakam"
    architecture "x64"
    startproject "Erebor"
    toolset(_OPTIONS["toolset"])
    
    configurations {
        "Debug",
        "DebugOptimized",
        "Release"
    }
    
    filter "toolset:clang"
        linkoptions { "-fuse-ld=lld" }
        
filter {}

group "Dependencies"
    include "Mahakam/vendor/GLFW"
    include "Mahakam/vendor/imgui"
    include "Mahakam/vendor/imguizmo"
    include "Mahakam/vendor/glad"
    include "Mahakam/vendor/yaml-cpp"
    include "Mahakam/vendor/bullet/build3"
group ""

include "Mahakam"
include "Erebor"
include "Sandbox"