project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"
    pic "on"

    targetdir ("bin/%{outputdir}")
    objdir ("bin-obj/%{outputdir}")

    files {
        "imconfig.h",
        "imgui.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_tables.cpp",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        "imgui_demo.cpp"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "options:target=linux"
        pic "on"
        systemversion "latest"

    filter "options:target=windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
