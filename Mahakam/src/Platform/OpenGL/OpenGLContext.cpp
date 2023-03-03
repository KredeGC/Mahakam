#include "Mahakam/mhpch.h"
#include "OpenGLContext.h"

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

#include <GLFW/glfw3.h>

#include <glad/gl.h>

namespace Mahakam
{
	OpenGLContext::OpenGLContext(void* window, void* proc) : m_Window((GLFWwindow*)window), m_ProcAddress(proc)
	{
		MH_ASSERT(m_Window, "Window is nullptr!");
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

		int status = gladLoadGL(glfwGetProcAddress);
		MH_ASSERT(status, "Could not initialize glad!");

		MH_ASSERT(glGetError, "Could not initialize OpenGL functions!");
	}
	
	void OpenGLContext::SwapBuffers()
	{
		MH_PROFILE_FUNCTION();

		glfwSwapBuffers(m_Window);
	}
}