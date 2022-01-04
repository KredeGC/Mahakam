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
		double lastFrameTime;
		static Application* instance;

		bool onWindowClose(WindowCloseEvent& event);
		bool onWindowResize(WindowResizeEvent& event);

	public:
		Application(const std::string& name = "Mahakam");
		virtual ~Application();

		void run();

		void close();

		void onEvent(Event& event);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		inline static Application& getInstance() { return *instance; }

		inline Window& getWindow() { return *window; }
	};

	Application* createApplication();
}