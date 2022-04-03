#include "mhpch.h"
#include "Application.h"

#include "Input.h"

#include "Mahakam/Renderer/Renderer.h"

namespace Mahakam
{
	Application* Application::instance = nullptr;

	Application::Application(const WindowProps& props)
	{
		MH_CORE_ASSERT(!instance, "Application instance already created!");
		instance = this;

		window = Window::Create(props);
		window->SetEventCallback(MH_BIND_EVENT(Application::OnEvent));

		Renderer::Init(props.width, props.height);

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
			double time = window->GetTime();
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

	void Application::PopLayer(Layer* layer)
	{
		layer->OnDetach();
		layerStack.PopLayer(layer);

		delete layer;
	}

	void Application::PopOverlay(Layer* overlay)
	{
		overlay->OnDetach();
		layerStack.PopOverlay(overlay);

		delete overlay;
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

		Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());

		return false;
	}
}