#include "Mahakam/mhpch.h"
#include "Mahakam/Core/Input.h"

#include "Mahakam/Core/Application.h"
#include "Mahakam/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Mahakam
{
	bool Input::IsKeyPressed(Key keycode)
	{
		Application* app = Application::GetInstance();
		Window& w = app->GetWindow();
		void* nativeW = w.GetNativeWindow();
		auto window = static_cast<GLFWwindow*>(nativeW);

		int state = glfwGetKey(window, (int)keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	};

	bool Input::IsMouseButtonPressed(MouseButton button)
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, (int)button);

		return state == GLFW_PRESS;
	};
	
	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePos();
		return x;
	};
	
	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return y;
	};

	Input::MousePos Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow().GetNativeWindow());

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { (float)xPos, (float)yPos };
	};
}