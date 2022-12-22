#pragma once
#include "Core.h"
#include "LayerStack.h"
#include "Timestep.h"
#include "Window.h"
#include "Types.h"

#include "Mahakam/Events/Event.h"
#include "Mahakam/Events/ApplicationEvent.h"

#include "Mahakam/ImGui/ImGuiLayer.h"

namespace Mahakam
{
	class Application
	{
	private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer = nullptr;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		double m_LastFrameTime = 0.0;
		static Application* s_Instance;

	public:
		Application(const WindowProps& props = WindowProps());
		virtual ~Application();

		void Run();

		void Close();

		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		static Application* GetInstance();

		inline static bool IsRunning() { return s_Instance->m_Running; }
		inline static bool IsMinimized() { return s_Instance->m_Minimized; }

		inline ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline Window& GetWindow() { return *m_Window; }

	private:
		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);
	};

	Application* CreateApplication();
}