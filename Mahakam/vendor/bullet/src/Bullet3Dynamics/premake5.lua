project "Bullet3Dynamics"
	language "C++"
    pic "on"
	kind "StaticLib"
	includedirs {".."}

	files {
		"**.cpp",
		"**.h"
	}