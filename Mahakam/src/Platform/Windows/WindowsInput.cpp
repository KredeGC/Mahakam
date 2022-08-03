#include "mhpch.h"
#include "Mahakam/Core/Input.h"
#include "Mahakam/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Mahakam
{
	//bool Input::IsKeyPressed(int keycode)
	MH_DEFINE_FUNC(Input::IsKeyPressed, bool, Key keycode)
	{
		Application* app = Application::GetInstance();
		Window& w = app->GetWindow();
		void* nativeW = w.GetNativeWindow();
		auto window = static_cast<GLFWwindow*>(nativeW);

		int state = glfwGetKey(window, (int)keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	};

	//bool Input::IsMouseButtonPressed(int button)
	MH_DEFINE_FUNC(Input::IsMouseButtonPressed, bool, int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	};
	
	//float Input::GetMouseX()
	MH_DEFINE_FUNC(Input::GetMouseX, float)
	{
		auto [x, y] = GetMousePos();
		return x;
	};
	
	//float Input::GetMouseY()
	MH_DEFINE_FUNC(Input::GetMouseY, float)
	{
		auto [x, y] = GetMousePos();
		return y;
	};

	//Input::MousePos Input::GetMousePos()
	MH_DEFINE_FUNC(Input::GetMousePos, Input::MousePos)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow().GetNativeWindow());

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { (float)xPos, (float)yPos };
	};
}