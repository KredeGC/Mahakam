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

    -- Windows
    filter "options:target=windows"
        systemversion "latest"
        
        defines {
            "MH_PLATFORM_WINDOWS",
            "WIN_EXPORT",
            "NOMINMAX"
        }
        
        flags { "MultiProcessorCompile" }

        postbuildcommands {
            "{COPYDIR} \"bin/%{outputdir}/Sandbox.dll\" \"../Erebor/runtime/\""
        }
    
    -- MinGW
    filter { "system:linux", "options:target=windows" }
        links { MinGWLinks }
        buildoptions { "-Wa,-mbig-obj", "-Wl,-allow-multiple-definition", "-fuse-ld=ld.lld-10" }

    -- Linux
    filter "options:target=linux"
        systemversion "latest"
        
        defines { "MH_PLATFORM_LINUX" }
        
        libdirs { LinuxLibDirs }
        
        links { LinuxLinks }

        postbuildcommands {
            "{COPYDIR} \"bin/%{outputdir}/libSandbox.so\" \"../Erebor/runtime/\""
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
        kind "ConsoleApp"
        
        flags { "LinkTimeOptimization" }
        
        defines {
            "MH_RELEASE",
            "MH_RUNTIME",
            "MH_STANDALONE"
        }
        runtime "Release"
        optimize "on"