project "BulletInverseDynamics"
    pic "on"
	kind "StaticLib"
    
	includedirs {
		".."
	}
    
	files {
		"IDMath.cpp",
		"MultiBodyTree.cpp",
		"details/MultiBodyTreeInitCache.cpp",
		"details/MultiBodyTreeImpl.cpp"
	}