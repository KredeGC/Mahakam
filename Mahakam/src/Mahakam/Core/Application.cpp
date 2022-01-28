#include "mhpch.h"
#include "Application.h"

#include "Input.h"

#include "Mahakam/Renderer/Renderer.h"


// TEMPORARY
#include <GLFW/glfw3.h>

namespace Mahakam
{
	Application* Application::instance = nullptr;

	Application::Application(const std::string& name)
	{
		MH_CORE_ASSERT(!instance, "Application instance already created!");
		instance = this;

		window = std::unique_ptr<Window>(Window::create({ name }));
		window->setEventCallback(MH_BIND_EVENT(Application::onEvent));

		Renderer::init(1600, 900);

		imGuiLayer = new ImGuiLayer();
		pushOverlay(imGuiLayer);
	}

	Application::~Application()
	{
		MH_PROFILE_FUNCTION();
	}

	void Application::run()
	{
		MH_PROFILE_FUNCTION();

		while (running)
		{
			// TEMPORARY
			double time = glfwGetTime();
			Timestep timestep = (float)(time - lastFrameTime);
			lastFrameTime = time;

			if (!minimized)
			{
				for (Layer* layer : layerStack)
					layer->onUpdate(timestep);
			}

			imGuiLayer->begin();
			for (Layer* layer : layerStack)
				layer->onImGuiRender();
			imGuiLayer->end();

			window->onUpdate();
		}
	}

	void Application::close()
	{
		running = false;
	}

	void Application::onEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.dispatchEvent<WindowCloseEvent>(MH_BIND_EVENT(Application::onWindowClose));
		dispatcher.dispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(Application::onWindowResize));

		for (auto iter = layerStack.end(); iter != layerStack.begin();)
		{
			if (event.handled)
				break;
			(*--iter)->onEvent(event);
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		layerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* overlay)
	{
		layerStack.pushOverlay(overlay);
		overlay->onAttach();
	}

	bool Application::onWindowClose(WindowCloseEvent& event)
	{
		running = false;
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& event)
	{
		if (event.getWidth() == 0 || event.getHeight() == 0)
		{
			minimized = true;
			return false;
		}

		minimized = false;

		Renderer::onWindowResie(event.getWidth(), event.getHeight());

		return false;
	}
}