#pragma once

#include <Mahakam.h>

namespace Mahakam::Editor
{
	class DockSpace
	{
	public:
		DockSpace() {}

		void Begin();
		void End();

		bool OnKeyPressed(KeyPressedEvent& event);

	private:
		void NewScene();
		void OpenScene();
		void SaveScene();
		void SaveSceneAs();
	};
}