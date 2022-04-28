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
		MH_DECLARE_FUNC(IsKeyPressed, bool, int keycode);
		
		MH_DECLARE_FUNC(IsMouseButtonPressed, bool, int button);
		MH_DECLARE_FUNC(GetMouseX, float);
		MH_DECLARE_FUNC(GetMouseY, float);
		MH_DECLARE_FUNC(GetMousePos, MousePos);
	};
}