#include "mhpch.h"
#include "WindowsInput.h"
#include "Mahakam/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Mahakam
{
	Input* Input::instance = new WindowsInput();

	bool WindowsInput::isKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		int state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::isMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		int state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}
	
	float WindowsInput::getMouseXImpl()
	{
		auto [x, y] = getMousePosImpl();
		return x;
	}
	
	float WindowsInput::getMouseYImpl()
	{
		auto [x, y] = getMousePosImpl();
		return y;
	}

	std::pair<float, float> WindowsInput::getMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { (float)xPos, (float)yPos };
	}
}