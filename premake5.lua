workspace "Mahakam"
    architecture "x64"
    startproject "Sandbox"
    
    configurations {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["assimp"] = "Mahakam/vendor/assimp"
IncludeDir["GLFW"] = "Mahakam/vendor/GLFW/include"
IncludeDir["glad"] = "Mahakam/vendor/glad/include"
IncludeDir["glm"] = "Mahakam/vendor/glm"
IncludeDir["imgui"] = "Mahakam/vendor/imgui"
IncludeDir["spdlog"] = "Mahakam/vendor/spdlog/include"
IncludeDir["stb_image"] = "Mahakam/vendor/stb_image"

group "Dependencies"
    os.execute("CMake \"Mahakam/vendor/assimp/CMakeLists.txt\"")
    --include "Mahakam/vendor/assimp"
    include "Mahakam/vendor/GLFW"
    include "Mahakam/vendor/imgui"
    include "Mahakam/vendor/glad"
group ""

project "Mahakam"
    location "Mahakam"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("bin/"..outputdir.."/%{prj.name}")
    objdir ("bin-obj/"..outputdir.."/%{prj.name}")
    
    pchheader "mhpch.h"
    pchsource "Mahakam/src/mhpch.cpp"
    
    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp"
    }
    
    includedirs {
        "%{prj.name}/src",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb_image}"
    }
    
    links {
        "GLFW",
        "glad",
        "ImGui",
        "assimp",
        "opengl32.lib"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }
    
    filter "system:windows"
        systemversion "latest"
        
        defines {
            "MH_PLATFORM_WINDOWS",
            "MH_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }
    
    filter "configurations:Debug"
        defines "MH_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        defines "MH_RELEASE"
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        defines "MH_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("bin/"..outputdir.."/%{prj.name}")
    objdir ("bin-obj/"..outputdir.."/%{prj.name}")
    
    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.c"
    }
    
    includedirs {
        "Mahakam/src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.glm}"
    }
    
    links {
        "Mahakam"
    }
    
    filter "system:windows"
        systemversion "latest"
        
        defines {
            "MH_PLATFORM_WINDOWS"
        }
    
    filter "configurations:Debug"
        defines "MH_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        defines "MH_RELEASE"
        runtime "Release"
        optimize "on"
    
    filter "configurations:Dist"
        defines "MH_DIST"
        runtime "Release"
        optimize "on"
