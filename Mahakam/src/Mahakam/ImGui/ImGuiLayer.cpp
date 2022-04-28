#include "mhpch.h"
#include "ImGuiLayer.h"

#include "Mahakam/Core/Application.h"

#include <imgui.h>

#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_opengl3.cpp>

#include <ImGuizmo.h>


// TEMPORARY
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

namespace Mahakam
{
	ImGuiLayer::ImGuiLayer() : Layer("ImGui")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	static void SetDarkTheme()
	{
		auto& style = ImGui::GetStyle().Colors;
		style[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);

		// Header
		style[ImGuiCol_Header] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
		style[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
		style[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);

		// Buttons
		style[ImGuiCol_Button] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
		style[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
		style[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);

		// Frame
		style[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);
		style[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.305f, 0.31f, 1.0f);
		style[ImGuiCol_FrameBgActive] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);

		// Tabs
		style[ImGuiCol_Tab] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
		style[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.385f, 0.39f, 1.0f);
		style[ImGuiCol_TabActive] = ImVec4(0.28f, 0.285f, 0.29f, 1.0f);
		style[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
		style[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.205f, 0.21f, 1.0f);

		// Title
		style[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
		style[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
		style[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.155f, 0.16f, 1.0f);
	}

	void ImGuiLayer::OnAttach()
	{
		MH_PROFILE_FUNCTION();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Disabled until ALT can be overriden
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Regular.ttf", 18.0f);

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkTheme();

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
		if (blockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event.handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		MH_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		MH_PROFILE_FUNCTION();

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