#include "mhpch.h"
#include "Mahakam/Core/Input.h"
#include "Mahakam/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Mahakam
{
	MH_DEFINE_FUNC(Input::IsKeyPressed, bool, int keycode)
	{
		Application* app = Application::GetInstance();
		Window& w = app->GetWindow();
		void* nativeW = w.GetNativeWindow();
		auto window = static_cast<GLFWwindow*>(nativeW);

		int state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	};

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}
	
	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePos();
		return x;
	}
	
	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return y;
	}

	std::pair<float, float> Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow().GetNativeWindow());

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { (float)xPos, (float)yPos };
	}
}