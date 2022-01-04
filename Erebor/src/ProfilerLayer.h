#pragma once

#include <Mahakam.h>

#include <imgui.h>

namespace Mahakam
{
	class ProfilerLayer : public Layer
	{
	private:
		bool open = true;

	public:
		ProfilerLayer() : Layer("Profiler") {}

		virtual void onImGuiRender() override;
	};
}