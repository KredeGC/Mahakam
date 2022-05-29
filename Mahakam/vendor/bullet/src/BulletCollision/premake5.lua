project "BulletCollision"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"
    
	includedirs { ".." }
    
	files {
		"*.cpp",
		"*.h",
		"BroadphaseCollision/*.cpp",
		"BroadphaseCollision/*.h",
		"CollisionDispatch/*.cpp",
                "CollisionDispatch/*.h",
		"CollisionShapes/*.cpp",
		"CollisionShapes/*.h",
		"Gimpact/*.cpp",
		"Gimpact/*.h",
		"NarrowPhaseCollision/*.cpp",
		"NarrowPhaseCollision/*.h",
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