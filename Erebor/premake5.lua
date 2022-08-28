project "Erebor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"
    
    targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")
    
    pchheader "ebpch.h"
    pchsource "src/ebpch.cpp"

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs { VendorIncludes }

    links { "Mahakam" }

    defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "options:target=windows"
        systemversion "latest"
        
        defines { "MH_PLATFORM_WINDOWS" }
        
        libdirs { LinuxLibDirs }
        
        links {
            "BulletDynamics",
            "BulletCollision",
            "LinearMath",
            "GLFW",
            "glad",
            "ImGui",
            "ImGuizmo",
            "pthread",
            "gdi32",
            "dwmapi",
            "stdc++fs",	--GCC versions 5.3 through 8.x need stdc++fs for std::filesystem
            "yaml-cpp",
            "assimp",
            "phonon"
        }
        
        flags { "MultiProcessorCompile" }
        
        postbuildcommands {
            "{COPYDIR} \"../Mahakam/vendor/steamaudio/lib/windows-x64/phonon.dll\" \"bin/%{outputdir}/\""
        }

    filter "options:target=linux"
        systemversion "latest"
        
        defines { "MH_PLATFORM_LINUX" }
        
        libdirs { LinuxLibDirs }
        
        links { LinuxLinks }
        
        postbuildcommands {
            "{COPYDIR} \"../Mahakam/vendor/steamaudio/lib/linux-x64/libphonon.so\" \"bin/%{outputdir}/\""
        }
    
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
            "MH_STANDALONE"
        }
        
        removefiles { "**/Panels/**" }
        
        runtime "Release"
        optimize "on"