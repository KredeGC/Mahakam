project "ImGuizmo"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

    files {
        "GraphEditor.h",
        "GraphEditor.cpp",
        "ImCurveEdit.h",
        "ImCurveEdit.cpp",
        "ImGradient.h",
        "ImGradient.cpp",
        "ImGuizmo.h",
        "ImGuizmo.cpp",
        "ImSequencer.h",
        "ImSequencer.cpp",
        "ImZoomSlider.h",
    }
    
    includedirs {
        "../imgui",
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }
    
    --[[links {
        "ImGui"
    }]]

    filter "system:linux"
        pic "on"
        systemversion "latest"

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
