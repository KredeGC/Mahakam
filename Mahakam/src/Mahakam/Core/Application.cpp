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

		window = std::unique_ptr<Window>(Window::Create({ name }));
		window->SetEventCallback(MH_BIND_EVENT(Application::OnEvent));

		Renderer::Init(1600, 900);

		imGuiLayer = new ImGuiLayer();
		PushOverlay(imGuiLayer);
	}

	Application::~Application()
	{
		MH_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::Run()
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
					layer->OnUpdate(timestep);
			}

			imGuiLayer->Begin();
			for (Layer* layer : layerStack)
				layer->OnImGuiRender();
			imGuiLayer->End();

			window->OnUpdate();
		}
	}

	void Application::Close()
	{
		running = false;
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.DispatchEvent<WindowCloseEvent>(MH_BIND_EVENT(Application::OnWindowClose));
		dispatcher.DispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(Application::OnWindowResize));

		for (auto iter = layerStack.end(); iter != layerStack.begin();)
		{
			if (event.handled)
				break;
			(*--iter)->OnEvent(event);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		if (event.GetWidth() == 0 || event.GetHeight() == 0)
		{
			minimized = true;
			return false;
		}

		minimized = false;

		Renderer::OnWindowResie(event.GetWidth(), event.GetHeight());

		return false;
	}
}