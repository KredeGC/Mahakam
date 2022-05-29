project "Bullet3Common"
	language "C++"
    pic "on"
	kind "StaticLib"
	includedirs {".."}

	files {
		"*.cpp",
		"*.h"
	}