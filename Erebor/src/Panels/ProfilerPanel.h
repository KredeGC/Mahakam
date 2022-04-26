#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class ProfilerPanel
	{
	private:
		bool open = true;
		bool recording = false;

	public:
		void OnImGuiRender();
	};
}