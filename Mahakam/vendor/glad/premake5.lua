project "glad"
	kind "StaticLib"
	language "C"
	staticruntime "off"
    pic "on"

	targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")

	files {
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c"
	}

	includedirs {
		"include"
	}

    -- Windows
	filter "system:windows"
		systemversion "latest"
    
    -- Linux
	filter "system:linux"
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
