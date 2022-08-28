workspace "Mahakam"
    architecture "x64"
    startproject "Erebor"
    
    configurations {
        "Debug",
        "DebugOptimized",
        "Release"
    }

newoption {
   trigger = "no-assimp",
   description = "Don't build assimp"
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

outputdir = "%{cfg.buildcfg}-%{_OPTIONS['target']}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["assimp"]            = "../Mahakam/vendor/assimp/include"
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
IncludeDir["yaml"]              = "../Mahakam/vendor/yaml-cpp/include"


if (_OPTIONS["target"] == "linux") then
    SteamAudioLibDir = "../Mahakam/vendor/steamaudio/lib/linux-x64"
else
    SteamAudioLibDir = "../Mahakam/vendor/steamaudio/lib/windows-x64"
end

AssimpLibDir = "../Mahakam/vendor/assimp/lib"
ZLibDir = "../Mahakam/vendor/assimp/contrib/zlib"

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

VendorLinks = {
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

MinGWLinks = {
    "gdi32",
    "dwmapi"
}

if (_OPTIONS["no-assimp"]) then
    LinuxLibDirs = {
        "assimp",
        SteamAudioLibDir
    }
    
    LinuxLinks = {
        "BulletDynamics",
        "BulletCollision",
        "LinearMath",
        "GLFW",
        "glad",
        "ImGui",
        "ImGuizmo",
        "Xrandr",
        "Xi",
        --"GLU",
        --"GL",
        "X11",
        "dl",
        "pthread",
        "stdc++fs",	--GCC versions 5.3 through 8.x need stdc++fs for std::filesystem
        "yaml-cpp",
        "assimp",
        "phonon"
    }
else
    LinuxLibDirs = {
        AssimpLibDir,
        ZLibDir,
        SteamAudioLibDir
    }
    
    LinuxLinks = {
        "BulletDynamics",
        "BulletCollision",
        "LinearMath",
        "GLFW",
        "glad",
        "ImGui",
        "ImGuizmo",
        "Xrandr",
        "Xi",
        --"GLU",
        --"GL",
        "X11",
        "dl",
        "pthread",
        "stdc++fs",	--GCC versions 5.3 through 8.x need stdc++fs for std::filesystem
        "yaml-cpp",
        "assimp",
        "zlibstatic",
        "phonon"
    }
end

-- Build ASSIMP for debug and release
if (_OPTIONS["no-assimp"] == false) then
    os.execute("cmake \"Mahakam/vendor/assimp/CMakeLists.txt\"")
    os.execute("cmake --build \"Mahakam/vendor/assimp/\" --config debug")
    os.execute("cmake --build \"Mahakam/vendor/assimp/\" --config release")
end

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