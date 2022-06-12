#pragma once

#include <Mahakam/Core/EntryPoint.h>

#if !defined(MH_STANDALONE_TITLE)
#error MH_STANDALONE_TITLE not defined
#endif

#if !defined(MH_STANDALONE_ICON)
#error MH_STANDALONE_ICON not defined
#endif

namespace Mahakam
{
	class StandaloneLayer : public Layer
	{
	public:
		StandaloneLayer() : Layer("Standalone") {}

		virtual void OnAttach() override
		{

		}

		virtual void OnDetach() override
		{

		}

		virtual void OnUpdate(Timestep dt) override
		{

		}

		virtual void OnImGuiRender() override
		{

		}

		virtual void OnEvent(Event& event) override
		{

		}
	};

	class StandaloneApplication : public Application
	{
	private:
		StandaloneLayer* m_Layer;

	public:
		StandaloneApplication()
			: Application({ MH_STANDALONE_TITLE, MH_STANDALONE_ICON })
		{
			// Create standalone layer
			m_Layer = new StandaloneLayer();

			PushOverlay(m_Layer);
		}
	};
}

extern Mahakam::Application* Mahakam::CreateApplication()
{
	return new Mahakam::StandaloneApplication();
}