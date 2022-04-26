#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam::Editor
{
	class ProfilerPanel : EditorWindow
	{
	private:
		bool open = true;
		bool recording = false;

	public:
		virtual void OnImGuiRender() override;
	};
}