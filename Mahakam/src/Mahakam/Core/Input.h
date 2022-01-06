#pragma once

#include "Core.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Mahakam
{
	class Input
	{
	public:
		static bool isKeyPressed(int keycode);

		static bool isMouseButtonPressed(int button);
		static float getMouseX();
		static float getMouseY();
		static std::pair<float, float> getMousePos();
	};
}