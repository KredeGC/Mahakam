project "spirv_cross"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"

	targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")

	files {
		"*.cpp",
		"include/**.h",
		"include/**.hpp"
	}

	includedirs {
		"include/spirv_cross"
	}

    filter "options:target=linux"
        systemversion "latest"

    filter "options:target=windows"
        systemversion "latest"
        
        buildoptions { "/bigobj" }
    
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