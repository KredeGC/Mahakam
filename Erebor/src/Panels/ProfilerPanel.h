#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class ProfilerPanel
	{
	private:
		bool open = true;

	public:
		void onImGuiRender();
	};
}