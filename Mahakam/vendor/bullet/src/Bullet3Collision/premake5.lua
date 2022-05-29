project "Bullet3Collision"
	language "C++"
    pic "on"
	kind "StaticLib"
	includedirs {".."}

	files {
		"**.cpp",
		"**.h"
	}