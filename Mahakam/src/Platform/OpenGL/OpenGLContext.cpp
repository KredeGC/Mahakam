#include "Mahakam/mhpch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>

#include <glad/glad.h>

namespace Mahakam
{
	OpenGLContext::OpenGLContext(void* window, void* proc) : m_Window((GLFWwindow*)window), m_ProcAddress(proc)
	{
		MH_CORE_ASSERT(m_Window, "Window is nullptr!");
	}
	
	void OpenGLContext::Init()
	{
		MH_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_Window);
		Reload();
	}

	void OpenGLContext::Reload()
	{
		MH_PROFILE_FUNCTION();

		int status = gladLoadGLLoader((GLADloadproc)m_ProcAddress);
		MH_CORE_ASSERT(status, "Could not initialize glad!");

		MH_CORE_ASSERT(glGetError, "Could not initialize OpenGL functions!");
	}
	
	void OpenGLContext::SwapBuffers()
	{
		MH_PROFILE_FUNCTION();

		glfwSwapBuffers(m_Window);
	}
}