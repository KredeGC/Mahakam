#include "Mahakam/mhpch.h"
#include "Window.h"

#include "Mahakam/Core/Log.h"

#include "Mahakam/Renderer/RendererAPI.h"

#include "Platform/GLFW/GLFWindow.h"
#include "Platform/Headless/HeadlessWindow.h"

namespace Mahakam
{
	//Scope<Window> Window::Create(const WindowProps& props)
	MH_DEFINE_FUNC(Window::CreateImpl, Scope<Window>, const WindowProps& props)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateScope<HeadlessWindow>(props);
		case RendererAPI::API::OpenGL:
			return CreateScope<GLFWindow>(props);
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	};
}