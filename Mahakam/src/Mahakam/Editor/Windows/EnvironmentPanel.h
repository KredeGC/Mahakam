#pragma once

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class EnvironmentPanel : EditorWindow
	{
	private:
		bool m_Open = true;

	public:
		EnvironmentPanel() = default;

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;
	};
}