project "Bullet3Collision"
	language "C++"
    pic "on"
	kind "StaticLib"
    
    targetdir ("bin/%{outputdir}/%{prj.name}")
    objdir ("bin-obj/%{outputdir}/%{prj.name}")
    
	includedirs {".."}

	files {
		"**.cpp",
		"**.h"
	}