workspace "Mahakam"
    architecture "x64"
    startproject "Erebor"
    
    configurations {
        "Debug",
        "DebugOptimized",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["assimp"]        = "Mahakam/vendor/assimp/include"
IncludeDir["entt"]          = "Mahakam/vendor/entt/include"
IncludeDir["GLFW"]          = "Mahakam/vendor/GLFW/include"
IncludeDir["glad"]          = "Mahakam/vendor/glad/include"
IncludeDir["glm"]           = "Mahakam/vendor/glm"
IncludeDir["imgui"]         = "Mahakam/vendor/imgui"
IncludeDir["imguizmo"]      = "Mahakam/vendor/imguizmo"
IncludeDir["robin_hood"]    = "Mahakam/vendor/robin_hood"
IncludeDir["spdlog"]        = "Mahakam/vendor/spdlog/include"
IncludeDir["stb_image"]     = "Mahakam/vendor/stb_image"
IncludeDir["yaml"]          = "Mahakam/vendor/yaml-cpp/include"

LinuxLinks = {
    "GLFW",
    "glad",
    "ImGui",
    "ImGuizmo",
    "Xrandr",
    "Xi",
    --"GLU",
    "GL",
    "X11",
    "dl",
    "pthread",
    "stdc++fs",	--GCC versions 5.3 through 8.x need stdc++fs for std::filesystem
    "yaml-cpp",
    "assimp",
    "zlibstatic"
}

VendorIncludes = {
    "%{prj.name}/src",
    "Mahakam/src",
    "%{IncludeDir.assimp}",
    "%{IncludeDir.entt}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.imgui}",
    "%{IncludeDir.imguizmo}",
    "%{IncludeDir.robin_hood}",
    "%{IncludeDir.spdlog}",
    "%{IncludeDir.yaml}"
}

-- Build ASSIMP for debug and release
os.execute("cmake \"Mahakam/vendor/assimp/CMakeLists.txt\"")
os.execute("cmake --build \"Mahakam/vendor/assimp/\" --config debug")
os.execute("cmake --build \"Mahakam/vendor/assimp/\" --config release")

group "Dependencies"
    include "Mahakam/vendor/GLFW"
    include "Mahakam/vendor/imgui"
    include "Mahakam/vendor/imguizmo"
    include "Mahakam/vendor/glad"
    include "Mahakam/vendor/yaml-cpp"
group ""

project "Mahakam"
    location "Mahakam"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"
    
    targetdir ("bin/%{outputdir}/%{prj.name}")
    objdir ("bin-obj/%{outputdir}/%{prj.name}")
    
    pchheader "mhpch.h"
    pchsource "Mahakam/src/mhpch.cpp"
    
    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/robin_hood/**.h",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp"
    }
    
    includedirs {
        "%{prj.name}/src",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.imguizmo}",
        "%{IncludeDir.robin_hood}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.yaml}"
    }
    
    links {
        "GLFW",
        "glad",
        "ImGui",
        "ImGuizmo",
        "yaml-cpp"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }
    
    -- Windows
    filter "system:windows"
        systemversion "latest"
        
        defines { "MH_PLATFORM_WINDOWS" }
        
        links {
            "opengl32.lib",
            "assimp-vc142-mt",
        }

    -- Linux
    filter "system:linux"
        systemversion "latest"
        
        defines { "MH_PLATFORM_LINUX" }
        
        libdirs {
            "./Mahakam/vendor/assimp/lib",
            "./Mahakam/vendor/assimp/contrib/zlib"
        }
        
        links { "zlibstatic" }
    
    -- Windows Debug
    filter { "system:windows", "configurations:Debug" }
        libdirs {
            "./Mahakam/vendor/assimp/lib/Debug",
            "./Mahakam/vendor/assimp/contrib/zlib/Debug"
        }
        
        links { "zlibstaticd" }
        
    -- Windows DebugOptimized and Release
    filter { "system:windows", "configurations:DebugOptimized or Release" }
        libdirs {
            "./Mahakam/vendor/assimp/lib/Release",
            "./Mahakam/vendor/assimp/contrib/zlib/Release"
        }
        
        links { "zlibstatic" }
    
    filter "configurations:Debug"
        defines "MH_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:DebugOptimized"
        defines "MH_DEBUG"
        runtime "Release"
        optimize "on"
        
    filter "configurations:Release"
        defines "MH_RELEASE"
        runtime "Release"
        optimize "on"

project "Erebor"
    location "Erebor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"
    
    targetdir ("bin/%{outputdir}/%{prj.name}")
    objdir ("bin-obj/%{outputdir}/%{prj.name}")
    
    pchheader "ebpch.h"
    pchsource "Erebor/src/ebpch.cpp"

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs { VendorIncludes }

    links { "Mahakam" }

    defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "system:windows"
        systemversion "latest"
        
        defines { "MH_PLATFORM_WINDOWS" }

    filter "system:linux"
        systemversion "latest"
        
        defines { "MH_PLATFORM_LINUX" }
        
        libdirs {
            "./Mahakam/vendor/assimp/lib",
            "./Mahakam/vendor/assimp/contrib/zlib"
        }
        
        links { LinuxLinks }
    
    filter "configurations:Debug"
        defines "MH_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:DebugOptimized"
        defines "MH_DEBUG"
        runtime "Release"
        optimize "on"
        
    filter "configurations:Release"
        defines "MH_RELEASE"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"
    
    targetdir ("bin/%{outputdir}/%{prj.name}")
    objdir ("bin-obj/%{outputdir}/%{prj.name}")
    
    pchheader "sbpch.h"
    pchsource "Sandbox/src/sbpch.cpp"

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs { VendorIncludes }

    links { "Mahakam" }

    defines { "_CRT_SECURE_NO_WARNINGS" }
    
    postbuildcommands {
        "{COPYDIR} \"../bin/%{outputdir}/%{prj.name}/\" \"../Erebor/runtime/\""
    }

    filter "system:windows"
        systemversion "latest"
        
        defines {
            "MH_PLATFORM_WINDOWS",
            "WIN_EXPORT"
        }

    filter "system:linux"
        systemversion "latest"
        
        defines { "MH_PLATFORM_LINUX" }
        
        libdirs {
            "./Mahakam/vendor/assimp/lib",
            "./Mahakam/vendor/assimp/contrib/zlib"
        }
        
        links { LinuxLinks }
    
    filter "configurations:Debug"
        defines "MH_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:DebugOptimized"
        defines "MH_DEBUG"
        runtime "Release"
        optimize "on"
        
    filter "configurations:Release"
        defines "MH_RELEASE"
        runtime "Release"
        optimize "on"