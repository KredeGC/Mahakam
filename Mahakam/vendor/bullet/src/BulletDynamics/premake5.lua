project "BulletDynamics"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"
    
    targetdir ("bin/%{outputdir}/%{prj.name}")
    objdir ("bin-obj/%{outputdir}/%{prj.name}")
    
	includedirs { ".." }
    
	files {
		"Dynamics/*.cpp",
        "Dynamics/*.h",
        "ConstraintSolver/*.cpp",
        "ConstraintSolver/*.h",
        "Featherstone/*.cpp",
        "Featherstone/*.h",
        "MLCPSolvers/*.cpp",
        "MLCPSolvers/*.h",
        "Vehicle/*.cpp",
        "Vehicle/*.h",
        "Character/*.cpp",
        "Character/*.h"
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