#pragma once

#ifndef MH_STANDALONE // MH_RUNTIME?
#include "Panels/DockSpace.h"
#endif

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class EditorLayer : public Layer
	{
	private:
#ifndef MH_STANDALONE
		DockSpace m_DockSpace;
#endif

	public:
		EditorLayer() : Layer("Editor") {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnWindowResized(WindowResizeEvent& event);
	};
}