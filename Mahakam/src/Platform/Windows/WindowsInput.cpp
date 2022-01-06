#include "mhpch.h"
#include "Mahakam/Core/Input.h"
#include "Mahakam/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Mahakam
{
	bool Input::isKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		int state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		int state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}
	
	float Input::getMouseX()
	{
		auto [x, y] = getMousePos();
		return x;
	}
	
	float Input::getMouseY()
	{
		auto [x, y] = getMousePos();
		return y;
	}

	std::pair<float, float> Input::getMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { (float)xPos, (float)yPos };
	}
}