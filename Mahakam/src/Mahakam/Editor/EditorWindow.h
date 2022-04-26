#pragma once

namespace Mahakam::Editor
{
	class EditorWindow
	{
	public:
		virtual ~EditorWindow() = default;

		virtual void OnImGuiRender() = 0;
	};
}