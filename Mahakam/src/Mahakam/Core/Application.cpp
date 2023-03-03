#include "Mahakam/mhpch.h"
#include "Application.h"

#include "Input.h"
#include "Log.h"
#include "Profiler.h"
#include "SharedLibrary.h"
#include "Timestep.h"

#include "Mahakam/Audio/AudioEngine.h"

#include "Mahakam/Events/ApplicationEvent.h"

#include "Mahakam/ImGui/ImGuiLayer.h"

#include "Mahakam/Physics/PhysicsEngine.h"

#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/RendererAPI.h"

#include "Mahakam/Serialization/YAMLGuard.h"

namespace Mahakam
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& logName, const WindowProps& props)
	{
		MH_PROFILE_FUNCTION();
        
        Log::Init(logName.c_str());
	    MH_INFO("Logging initialized");

		MH_ASSERT(!s_Instance, "Application instance already created!");
		s_Instance = this;

		m_Window = Window::Create(props);
		m_Window->SetEventCallback(MH_BIND_EVENT(Application::OnEvent));

		YAMLGuard::Init();

		Renderer::Init(props.Width, props.Height);

		AudioEngine::Init();

		PhysicsEngine::Init();

		if (RendererAPI::GetAPI() != RendererAPI::API::None)
		{
			m_ImGuiLayer = new ImGuiLayer();
			PushOverlay(m_ImGuiLayer);
		}
	}

	Application::~Application()
	{
		MH_PROFILE_FUNCTION();

		for (Layer* layer : m_LayerStack)
			layer->OnDetach();

		AudioEngine::Shutdown();

		PhysicsEngine::Shutdown();

		Renderer::Shutdown();

		YAMLGuard::Shutdown();

        MH_INFO("Logging uninitialized");
        Log::Shutdown();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			double time = m_Window->GetTime();
			Timestep timestep = (float)(time - m_LastFrameTime);
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			if (m_ImGuiLayer)
			{
				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();

#ifdef MH_ENABLE_PROFILING
			Profiler::ClearResults();
#endif
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.DispatchEvent<WindowCloseEvent>(MH_BIND_EVENT(Application::OnWindowClose));
		dispatcher.DispatchEvent<WindowResizeEvent>(MH_BIND_EVENT(Application::OnWindowResize));

		for (auto iter = m_LayerStack.end(); iter != m_LayerStack.begin();)
		{
			if (event.handled)
				break;
			(*--iter)->OnEvent(event);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::PopLayer(Layer* layer)
	{
		layer->OnDetach();
		m_LayerStack.PopLayer(layer);

		delete layer;
	}

	void Application::PopOverlay(Layer* overlay)
	{
		overlay->OnDetach();
		m_LayerStack.PopOverlay(overlay);

		delete overlay;
	}

	Application* Application::GetInstance()
	{
		return s_Instance;
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		if (event.GetWidth() == 0 || event.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;

		Renderer::OnWindowResize(event.GetWidth(), event.GetHeight());

		return false;
	}
}