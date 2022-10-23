project "glslang"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"

	targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")

	files {
		"**.cpp",
		"**.h",
		"**.hpp"
	}

	includedirs {
		".",
        "include"
	}

    removefiles {
        "glslang/OSDependent/Windows/**",
        "glslang/OSDependent/Web/**",
        "glslang/HLSL/**",
    }

    filter "options:target=linux"
        systemversion "latest"

    filter "options:target=windows"
        systemversion "latest"
    
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