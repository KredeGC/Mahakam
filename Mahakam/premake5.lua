project "Mahakam"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"
    
    targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")
    
    pchheader "Mahakam/mhpch.h"
    pchsource "src/Mahakam/mhpch.cpp"
    
    files {
        "src/**.h",
        "src/**.cpp",
        "vendor/glm/glm/**.hpp",
        "vendor/glm/glm/**.inl",
        "vendor/miniaudio/**.h",
        "vendor/robin_hood/**.h",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp"
    }
    
    includedirs {
        "src",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.bullet}",
        "%{IncludeDir.bullet_dynamics}",
        "%{IncludeDir.bullet_collision}",
        "%{IncludeDir.linear_math}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.imguizmo}",
        "%{IncludeDir.miniaudio}",
        "%{IncludeDir.robin_hood}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.steamaudio}",
        "%{IncludeDir.yaml}"
    }
    
    libdirs { SteamAudioLibDir }
    
    links {
        "BulletDynamics",
        "BulletCollision",
        "LinearMath",
        "GLFW",
        "glad",
        "ImGui",
        "ImGuizmo",
        "phonon",
        "yaml-cpp"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE",
        "MH_BUILD"
    }
    
    -- Windows
    filter "system:windows"
        systemversion "latest"
        
        removefiles { "**/Linux/**" }
        
        flags { "MultiProcessorCompile" }
        
        defines {
            "MH_PLATFORM_WINDOWS",
            "NOMINMAX"
        }
        
        links {
            "opengl32.lib",
            "assimp-vc142-mt"
        }

    -- Linux
    filter "system:linux"
        systemversion "latest"
        
        removefiles { "**/Windows/WindowsFileUtility.cpp" }
        
        defines { "MH_PLATFORM_LINUX" }
        
        libdirs {
            "%{AssimpLibDir}",
            "%{ZLibDir}"
        }
        
        links {
            "assimp",
            "zlibstatic"
        }
    
    -- Windows Debug
    filter { "system:windows", "configurations:Debug" }
        libdirs {
            "%{AssimpLibDir}/Debug",
            "%{ZLibDir}/Debug"
        }
        
        links { "zlibstaticd" }
        
    -- Windows DebugOptimized and Release
    filter { "system:windows", "configurations:DebugOptimized or Release" }
        libdirs {
            "%{AssimpLibDir}/Release",
            "%{ZLibDir}/Release"
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
        flags { "LinkTimeOptimization" }
        
        defines {
            "MH_RELEASE",
            "MH_RUNTIME",
            "MH_STANDALONE"
        }
        
        removefiles { "**/Editor/**" }
        
        runtime "Release"
        optimize "on"