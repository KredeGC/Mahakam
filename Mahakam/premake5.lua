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
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp",
        "vendor/tiny_gltf/tiny_gltf/**.h",
        "vendor/tiny_gltf/tiny_gltf/**.hpp",
        "vendor/tiny_gltf/tiny_gltf/**.cpp"
    }
    
    includedirs {
        "src",
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
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.steamaudio}",
        "%{IncludeDir.tiny_gltf}",
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
        "yaml-cpp",
        "phonon"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE",
        "MH_BUILD"
    }
    
    -- Windows
    filter "options:target=windows"
        systemversion "latest"
        
        buildoptions { "/bigobj" }
        
        removefiles { "**/Linux/**" }
        
        flags { "MultiProcessorCompile" }
        
        defines {
            "MH_PLATFORM_WINDOWS",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
        }
        
        links { "opengl32.lib" }
    
    -- Linux
    filter "options:target=linux"
        systemversion "latest"
        
        removefiles { "**/Windows/WindowsFileUtility.cpp" }
        
        defines { "MH_PLATFORM_LINUX" }
    
    -- Unity build
    filter "options:unity"
        unity "on"
        
    -- Standalone
    filter "options:standalone"
        removefiles { "**/Editor/**" }
        defines { "MH_STANDALONE" }
    
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
        
        defines { "MH_RELEASE" }
        
        runtime "Release"
        optimize "on"