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
        "{MKDIR} \"bin/%{outputdir}/\"",
        "{COPYDIR} \"bin/%{outputdir}/.\" \"../Erebor/runtime/\""
    }

    -- Windows
    filter "options:target=windows"
        systemversion "latest"
        
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
        buildoptions { "-Wa,-mbig-obj", "-Wl,-allow-multiple-definition", "-fuse-ld=lld" }
        linkoptions { "-fuse-ld=lld" }

    -- Linux
    filter "options:target=linux"
        systemversion "latest"
        
        defines { "MH_PLATFORM_LINUX" }
        
        libdirs { LinuxLibDirs }
        
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
        kind "ConsoleApp"
        
        flags { "LinkTimeOptimization" }
        
        defines {
            "MH_RELEASE",
            "MH_RUNTIME",
            "MH_STANDALONE"
        }
        runtime "Release"
        optimize "on"