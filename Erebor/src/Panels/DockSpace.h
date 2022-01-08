#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class DockSpace
	{
	public:
		DockSpace() {}

		void onImGuiRender();
	};
}