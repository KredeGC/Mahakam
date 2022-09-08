project "yaml-cpp"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"

	targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")

	files {
		"src/**.h",
		"src/**.cpp",
		
		"include/**.h"
	}

	includedirs {
		"include"
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