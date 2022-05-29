project "Bullet3Geometry"
	language "C++"
    pic "on"
	kind "StaticLib"
	includedirs {".."}

	files {
		"**.cpp",
		"**.h"
	}