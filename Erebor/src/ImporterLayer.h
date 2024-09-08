#pragma once

#include "Panels/DockSpace.h"

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class ImporterLayer : public Layer
	{
	private:
		DockSpace m_DockSpace;

	public:
		ImporterLayer() : Layer("Editor") {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	};
}