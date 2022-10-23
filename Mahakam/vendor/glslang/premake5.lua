project "glslang"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"

	targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")

	files {
        "glslang/**.cpp",
        "OGLCompilersDLL/InitializeDll.cpp",
        "SPIRV/**.cpp",
        "Standalone/**.cpp"
	}

	includedirs {
		".",
        "include",
        "Standalone",
        "OGLCompilersDLL"
	}

    removefiles {
        "glslang/OSDependent/**"
    }
    
    defines {
        "ENABLE_HLSL"
    }

    filter "options:target=linux"
        systemversion "latest"
        
        files {
            "glslang/OSDependent/Unix/ossource.cpp"
        }

    filter "options:target=windows"
        systemversion "latest"
        
        files {
            "glslang/OSDependent/Windows/ossource.cpp"
        }
    
    -- Unity build
    filter "options:unity"
        unity "on"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:DebugOptimized"
        runtime "Release"
        optimize "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"