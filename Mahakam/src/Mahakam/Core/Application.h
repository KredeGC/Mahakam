#pragma once
#include "Core.h"
#include "LayerStack.h"
#include "Mahakam/Events/Event.h"
#include "Mahakam/Events/ApplicationEvent.h"
#include "Window.h"

#include "Timestep.h"

#include "Mahakam/ImGui/ImGuiLayer.h"

namespace Mahakam
{
	class Application
	{
	private:
		std::unique_ptr<Window> window;
		ImGuiLayer* imGuiLayer;
		bool running = true;
		bool minimized = false;
		LayerStack layerStack;
		double lastFrameTime = 0.0;
		static Application* instance;

	public:
		Application(const std::string& name = "Mahakam");
		virtual ~Application();

		void Run();

		void Close();

		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& GetInstance() { return *instance; }

		inline static bool IsRunning() { return instance->running; }

		inline Window& GetWindow() { return *window; }

	private:
		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);
	};

	Application* CreateApplication();
}