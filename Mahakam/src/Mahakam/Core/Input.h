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
		static bool IsKeyPressed(Key keycode);
		
		static bool IsMouseButtonPressed(MouseButton button);
		static float GetMouseX();
		static float GetMouseY();
		static MousePos GetMousePos();
	};
}