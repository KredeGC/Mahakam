#pragma once

#include "Panels/DockSpace.h"

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class EditorLayer : public Layer
	{
	private:
		DockSpace m_DockSpace;

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