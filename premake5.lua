workspace "Mahakam"
    architecture "x64"
    startproject "Sandbox"
    
    configurations {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["assimp"]    = "Mahakam/vendor/assimp/include"
IncludeDir["entt"]      = "Mahakam/vendor/entt/include"
IncludeDir["GLFW"]      = "Mahakam/vendor/GLFW/include"
IncludeDir["glad"]      = "Mahakam/vendor/glad/include"
IncludeDir["glm"]       = "Mahakam/vendor/glm"
IncludeDir["imgui"]     = "Mahakam/vendor/imgui"
IncludeDir["spdlog"]    = "Mahakam/vendor/spdlog/include"
IncludeDir["stb_image"] = "Mahakam/vendor/stb_image"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["yaml"]      = "Mahakam/vendor/yaml-cpp/include"

LinkDir = {}
LinkDir["VulkanSDK"]    = "%{VULKAN_SDK}/Lib"
LinkDir["VulkanSDK_Debug"] = "%{wks.location}/Mahakam/vendor/VulkanSDK/Lib"
LinkDir["VulkanSDK_DebugDLL"] = "%{wks.location}/Mahakam/vendor/VulkanSDK/Bin"

Links = {}
Links["Vulkan"] = "%{LinkDir.VulkanSDK}/vulkan-1.lib"
Links["VulkanUtils"] = "%{LinkDir.VulkanSDK}/VkLayer_utils.lib"

Links["ShaderC_Debug"] = "%{LinkDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Links["SPIRV_Cross_Debug"] = "%{LinkDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Links["SPIRV_Cross_GLSL_Debug"] = "%{LinkDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Links["SPIRV_Tools_Debug"] = "%{LinkDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Links["ShaderC_Release"] = "%{LinkDir.VulkanSDK}/shaderc_shared.lib"
Links["SPIRV_Cross_Release"] = "%{LinkDir.VulkanSDK}/spirv-cross-core.lib"
Links["SPIRV_Cross_GLSL_Release"] = "%{LinkDir.VulkanSDK}/spirv-cross-glsl.lib"

group "Dependencies/Assimp"
    os.execute("cmake \"Mahakam/vendor/assimp/CMakeLists.txt\"")
    --os.execute("cmake --build \"Mahakam/vendor/assimp/\"")
    
    externalproject "assimp"
        location "Mahakam/vendor/assimp/code"
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"
        uuid "A8CB2FE6-9AD5-3CE2-8D32-B7DFAF8EC735"
    
    externalproject "zlibstatic"
        location "Mahakam/vendor/assimp/contrib/zlib"
        kind "StaticLib"
        language "C"
        uuid "A1367BDB-5B32-37E6-9FEA-6F7654E7330B"
        

group "Dependencies"
    --include "Mahakam/vendor/assimp"
    include "Mahakam/vendor/GLFW"
    include "Mahakam/vendor/imgui"
    include "Mahakam/vendor/glad"
    include "Mahakam/vendor/yaml-cpp"
group ""

project "Mahakam"
    location "Mahakam"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("bin/"..outputdir.."/%{prj.name}")
    objdir ("bin-obj/"..outputdir.."/%{prj.name}")
    
    pchheader "mhpch.h"
    pchsource "Mahakam/src/mhpch.cpp"
    
    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp"
    }
    
    includedirs {
        "%{prj.name}/src",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.yaml}"
    }
    
    links {
        "GLFW",
        "glad",
        "ImGui",
        "assimp",
        "yaml-cpp",
        "opengl32.lib"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }
    
    filter "system:windows"
        systemversion "latest"
        
        defines {
            "MH_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }
    
    filter "configurations:Debug"
        defines "MH_DEBUG"
        runtime "Debug"
        symbols "on"
        
        links {
			"%{Links.ShaderC_Debug}",
			"%{Links.SPIRV_Cross_Debug}",
			"%{Links.SPIRV_Cross_GLSL_Debug}"
		}
        
    filter "configurations:Release"
        defines "MH_RELEASE"
        runtime "Release"
        optimize "on"
        
        links {
            "%{Links.ShaderC_Release}",
			"%{Links.SPIRV_Cross_Release}",
			"%{Links.SPIRV_Cross_GLSL_Release}"
        }

project "Erebor"
    location "Erebor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/"..outputdir.."/%{prj.name}")
    objdir ("bin-obj/"..outputdir.."/%{prj.name}")
    
    pchheader "ebpch.h"
    pchsource "Erebor/src/ebpch.cpp"

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.c"
    }

    includedirs {
        "%{prj.name}/src",
        "Mahakam/src",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.glm}"
    }

    links {
        "Mahakam"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"
        
        defines {
            "MH_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "MH_DEBUG"
        runtime "Debug"
        symbols "on"
        
        --[[postbuildcommands {
			"{COPYDIR} \"%{LinkDir.VulkanSDK_DebugDLL}\" \"%{cfg.targetdir}\""
		}]]
        
    filter "configurations:Release"
        defines "MH_RELEASE"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("bin/"..outputdir.."/%{prj.name}")
    objdir ("bin-obj/"..outputdir.."/%{prj.name}")
    
    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.c"
    }
    
    includedirs {
        "%{prj.name}/src",
        "Mahakam/src",
        "%{IncludeDir.assimp}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.glm}"
    }
    
    links {
        "Mahakam"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }
    
    filter "system:windows"
        systemversion "latest"
        
        defines {
            "MH_PLATFORM_WINDOWS"
        }
    
    filter "configurations:Debug"
        defines "MH_DEBUG"
        runtime "Debug"
        symbols "on"
        
    filter "configurations:Release"
        defines "MH_RELEASE"
        runtime "Release"
        optimize "on"
