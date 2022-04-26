#pragma once

#include "Core.h"
#include "SharedLibrary.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Mahakam
{
	class Input
	{
	public:
		MH_DECLARE_FUNC(IsKeyPressed, bool, int keycode);
		
		static bool IsMouseButtonPressed(int button);
		static float GetMouseX();
		static float GetMouseY();
		static std::pair<float, float> GetMousePos();
	};
}