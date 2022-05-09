#pragma once

#include <Mahakam.h>

namespace Mahakam::Editor
{
	class AssetManagerPanel : EditorWindow
	{
	private:
		bool m_Open = true;

	public:
		AssetManagerPanel();

		virtual void OnImGuiRender() override;
	};
}