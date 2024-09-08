#pragma once

#include <Mahakam/Mahakam.h>

#include <imgui/imgui.h>

namespace Mahakam::Editor
{
	class BuildPanel : public EditorWindow
	{
	private:
		bool m_Open = true;

	public:
		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;
	};
}