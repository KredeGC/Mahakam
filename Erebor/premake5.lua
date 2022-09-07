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
    
    libdirs { VendorLibDirs }

    links { VendorLinks }

    defines { "_CRT_SECURE_NO_WARNINGS" }
    
    postbuildcommands {
        "{COPYDIR} \"../Mahakam/vendor/steamaudio/lib/".._OPTIONS["target"].."-x64/.\" \"bin/%{outputdir}/\""
    }
    
    -- Windows
    filter "options:target=windows"
        systemversion "latest"
        
        flags { "MultiProcessorCompile" }
        
        defines {
            "MH_PLATFORM_WINDOWS",
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
        
        defines { "MH_PLATFORM_LINUX" }
        
        libdirs { LinuxLibDirs }
        
        links { LinuxLinks }
        
    -- Standalone
    filter "options:standalone"
        removefiles { "**/Panels/**" }
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