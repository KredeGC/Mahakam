#include "Mahakam/mhpch.h"
#include "Mahakam/Core/Input.h"
#include "Mahakam/Core/Application.h"

#ifndef MH_HEADLESS
#include <GLFW/glfw3.h>
#endif

namespace Mahakam
{
	//bool Input::IsKeyPressed(int keycode)
	MH_DEFINE_FUNC(Input::IsKeyPressed, bool, Key keycode)
	{
#ifdef MH_HEADLESS
		return false;
#else
		Application* app = Application::GetInstance();
		Window& w = app->GetWindow();
		void* nativeW = w.GetNativeWindow();
		auto window = static_cast<GLFWwindow*>(nativeW);

		int state = glfwGetKey(window, (int)keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
#endif // MH_HEADLESS
	};

	//bool Input::IsMouseButtonPressed(int button)
	MH_DEFINE_FUNC(Input::IsMouseButtonPressed, bool, MouseButton button)
	{
#ifdef MH_HEADLESS
		return false;
#else
		auto window = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, (int)button);

		return state == GLFW_PRESS;
#endif // MH_HEADLESS
	};
	
	//float Input::GetMouseX()
	MH_DEFINE_FUNC(Input::GetMouseX, float)
	{
#ifdef MH_HEADLESS
		return 0.0f;
#else
		auto [x, y] = GetMousePos();
		return x;
#endif // MH_HEADLESS
	};
	
	//float Input::GetMouseY()
	MH_DEFINE_FUNC(Input::GetMouseY, float)
	{
#ifdef MH_HEADLESS
		return 0.0f;
#else
		auto [x, y] = GetMousePos();
		return y;
#endif // MH_HEADLESS
	};

	//Input::MousePos Input::GetMousePos()
	MH_DEFINE_FUNC(Input::GetMousePos, Input::MousePos)
	{
#ifdef MH_HEADLESS
		return { 0.0f, 0.0f };
#else
		auto window = static_cast<GLFWwindow*>(Application::GetInstance()->GetWindow().GetNativeWindow());

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { (float)xPos, (float)yPos };
#endif // MH_HEADLESS
	};
}