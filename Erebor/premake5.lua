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

    filter "options:target=windows"
        systemversion "latest"
        
        defines {
            "MH_PLATFORM_WINDOWS",
            "NOMINMAX"
        }
        
        flags { "MultiProcessorCompile" }

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
        flags { "LinkTimeOptimization" }
        
        defines {
            "MH_RELEASE",
            "MH_STANDALONE"
        }
        
        removefiles { "**/Panels/**" }
        
        runtime "Release"
        optimize "on"