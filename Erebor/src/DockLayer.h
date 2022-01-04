#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class DockLayer : public Layer
	{
	public:
		DockLayer() : Layer("Editor") {}

		virtual void onImGuiRender() override;
	};
}