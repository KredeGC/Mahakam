project "Sandbox"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"
    
    targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")
    
    pchheader "sbpch.h"
    pchsource "src/sbpch.cpp"

    files {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs { VendorIncludes }
    
    libdirs { VendorLibDirs }

    links { VendorLinks }

    defines { "_CRT_SECURE_NO_WARNINGS" }
    
    postbuildcommands {
        "{MKDIR} \"../Erebor/runtime/\"",
        "{COPYDIR} \"bin/%{outputdir}/.\" \"../Erebor/runtime/\""
    }

    -- Windows
    filter "options:target=windows"
        systemversion "latest"
        
        buildoptions { "/bigobj" }
        
        flags { "MultiProcessorCompile" }
        
        defines {
            "MH_PLATFORM_WINDOWS",
            "MH_WIN_EXPORT",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
        }
    
    -- MinGW
    filter { "system:linux", "options:target=windows" }
        links { MinGWLinks }
        buildoptions { "-Wa,-mbig-obj", "-Wl,-allow-multiple-definition" }

    -- Linux
    filter "options:target=linux"
        systemversion "latest"
        
        buildoptions { "-Wa,-mbig-obj" }
        
        defines { "MH_PLATFORM_LINUX" }
        
        libdirs { LinuxLibDirs }
        
        links { LinuxLinks }
    
    -- Unity build
    filter "options:unity"
        unity "on"
        
    -- Standalone
    filter "options:standalone"
        kind "ConsoleApp"
        
        defines { "MH_STANDALONE" }
    
        postbuildcommands {
            "{COPYDIR} \"../Mahakam/vendor/steamaudio/lib/".._OPTIONS["target"].."-x64/.\" \"bin/%{outputdir}/\""
        }
    
    -- Configs
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