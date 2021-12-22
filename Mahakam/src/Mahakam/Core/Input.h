#pragma once

#include "Core.h"

namespace Mahakam
{
	class Input
	{
	private:
		static Input* instance;

	protected:
		virtual bool isKeyPressedImpl(int keycode) = 0;

		virtual bool isMouseButtonPressedImpl(int button) = 0;
		virtual float getMouseXImpl() = 0;
		virtual float getMouseYImpl() = 0;
		virtual std::pair<float, float> getMousePosImpl() = 0;

	public:
		inline static bool isKeyPressed(int keycode) { return instance->isKeyPressedImpl(keycode); }

		inline static bool isMouseButtonPressed(int button) { return instance->isKeyPressedImpl(button); }
		inline static float getMouseX() { return instance->getMouseXImpl(); }
		inline static float getMouseY() { return instance->getMouseYImpl(); }
		inline static std::pair<float, float> getMousePos() { return instance->getMousePosImpl(); }
	};
}