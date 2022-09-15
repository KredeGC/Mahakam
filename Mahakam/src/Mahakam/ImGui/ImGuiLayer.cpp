#include "Mahakam/mhpch.h"
#include "ImGuiLayer.h"

#include "Mahakam/Core/Application.h"
#include "Mahakam/Core/Profiler.h"

#include <imgui/imgui.h>

#include <imgui/backends/imgui_impl_glfw.cpp>
#include "imgui_impl_glad.h" //#include <imgui/backends/imgui_impl_opengl3.cpp>

#include <imguizmo/ImGuizmo.h>

namespace Mahakam
{
	ImGuiLayer::ImGuiLayer() : Layer("ImGui")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	static void SetDarkTheme(ImGuiStyle& style)
	{
		auto& colors = style.Colors;
		colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.145f, 0.15f, 1.0f);

		// Header
		colors[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);

		// Buttons
		colors[ImGuiCol_Button] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);

		// Frame
		colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);

		// Borders
		colors[ImGuiCol_Border] = ImVec4(0.1f, 0.09f, 0.08f, 1.0f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.2f, 0.18f, 0.14f, 0.5f);

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.385f, 0.39f, 1.0f);
		colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.285f, 0.29f, 1.0f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

		// Popups
		colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.125f, 0.13f, 1.0f);

		// DragDrop
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.8f, 0.7f, 0.2f, 1.0f);

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
	}

	void ImGuiLayer::OnAttach()
	{
		MH_PROFILE_FUNCTION();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Disabled until ALT can be overriden
		io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Default font
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Regular.ttf", 16.0f);

		// Font icons
		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 16.0f; // Use if you want to make the icon monospaced
		static const ImWchar icon_ranges[] = { 0xe800, 0xf02e, 0 };
		io.Fonts->AddFontFromFileTTF("assets/fonts/IcoFont/icofont.ttf", 16.0f, &config, icon_ranges);

		// Mono font
		ImFontConfig monoConfig;
		monoConfig.GlyphMinAdvanceX = 8.0f;
		monoConfig.GlyphMaxAdvanceX = 8.0f;
		m_MonoFont = io.Fonts->AddFontFromFileTTF("assets/fonts/ProggyVector/ProggyVector Regular.ttf", 15.0f, &monoConfig);

		io.Fonts->Build();

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.WindowBorderSize = 0.0f;
		style.PopupRounding = 2.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameRounding = 2.0f;
		style.FrameBorderSize = 1.0f;
		style.ChildRounding = 2.0f;
		style.ChildBorderSize = 1.0f;
		style.TabRounding = 2.0f;
		style.TabBorderSize = 0.0f;
		style.ScrollbarRounding = 2.0f;
		style.ScrollbarSize = 14.0f;

		SetDarkTheme(style);

		Application* app = Application::GetInstance();
		GLFWwindow* window = static_cast<GLFWwindow*>(app->GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 430");
	}

	void ImGuiLayer::OnDetach()
	{
		MH_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event.handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application* app = Application::GetInstance();
		io.DisplaySize = ImVec2((float)app->GetWindow().GetWidth(), (float)app->GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}