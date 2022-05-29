project "LinearMath"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"
    
	includedirs { ".." }
    
	files {
		"*.cpp",
		"*.h",
		"TaskScheduler/*.cpp",
		"TaskScheduler/*.h"
	}
    
    filter "configurations:Debug"
        defines {"_DEBUG=1"}
        floatingpoint "Fast"
        flags { "NoMinimalRebuild" }
        runtime "Debug"
        symbols "on"
        
    filter "configurations:DebugOptimized"
        vectorextensions "SSE2"
        floatingpoint "Fast"
        flags { "NoMinimalRebuild" }
        runtime "Release"
        optimize "on"
        
    filter "configurations:Release"
        vectorextensions "SSE2"
        floatingpoint "Fast"
        flags { "NoMinimalRebuild" }
        runtime "Release"
        optimize "on"