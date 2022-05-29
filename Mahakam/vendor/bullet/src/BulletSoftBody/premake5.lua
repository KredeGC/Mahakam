project "BulletSoftBody"
    pic "on"
	kind "StaticLib"
	
	includedirs {
		"..",
	}
    
	files {
		"**.cpp",
                "BulletReducedDeformableBody/**.cpp",
		"**.h"
	}