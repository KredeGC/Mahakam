#pragma once

#include "Core.h"
#include "SharedLibrary.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Mahakam
{
	class Input
	{
	private:
		using MousePos = std::pair<float, float>;

	public:
		MH_DECLARE_FUNC(IsKeyPressed, bool, Key keycode);
		
		MH_DECLARE_FUNC(IsMouseButtonPressed, bool, MouseButton button);
		MH_DECLARE_FUNC(GetMouseX, float);
		MH_DECLARE_FUNC(GetMouseY, float);
		MH_DECLARE_FUNC(GetMousePos, MousePos);
	};
}