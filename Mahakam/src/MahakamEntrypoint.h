#pragma once

#include <Mahakam/Core/EntryPoint.h>

#define MH_STANDALONE_TITLE "Title"
#define MH_STANDALONE_ICON "res/internal/icons/icon-editor.png"

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