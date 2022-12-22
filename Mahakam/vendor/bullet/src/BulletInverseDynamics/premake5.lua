project "BulletInverseDynamics"
    pic "on"
	kind "StaticLib"
    
    targetdir ("bin/%{outputdir}/%{prj.name}")
    objdir ("bin-obj/%{outputdir}/%{prj.name}")
    
	includedirs {
		".."
	}
    
	files {
		"IDMath.cpp",
		"MultiBodyTree.cpp",
		"details/MultiBodyTreeInitCache.cpp",
		"details/MultiBodyTreeImpl.cpp"
	}