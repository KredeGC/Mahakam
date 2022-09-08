project "ImGuizmo"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"

    targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")

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

    filter "options:target=linux"
        systemversion "latest"

    filter "options:target=windows"
        systemversion "latest"
    
    -- Unity build
    filter "options:unity"
        unity "on"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:DebugOptimized"
        runtime "Release"
        optimize "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
