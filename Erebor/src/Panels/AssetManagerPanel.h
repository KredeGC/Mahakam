#pragma once

#include <Mahakam/Mahakam.h>

namespace Mahakam::Editor
{
	class AssetManagerPanel : EditorWindow
	{
	private:
		bool m_Open = true;

		std::string m_SearchString;

	public:
		AssetManagerPanel();

		virtual bool IsOpen() const override { return m_Open; }

		virtual void OnImGuiRender() override;
	};
}