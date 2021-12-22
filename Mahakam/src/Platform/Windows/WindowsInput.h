#pragma once

#include "Mahakam/Core/Input.h"

namespace Mahakam
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool isKeyPressedImpl(int keycode) override;

		virtual bool isMouseButtonPressedImpl(int button) override;
		virtual float getMouseXImpl() override;
		virtual float getMouseYImpl() override;
		virtual std::pair<float, float> getMousePosImpl() override;
	};
}