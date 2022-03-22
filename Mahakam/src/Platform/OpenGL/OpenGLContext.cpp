#include "mhpch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>

#include <glad/glad.h>

namespace Mahakam
{
	OpenGLContext::OpenGLContext(void* window, void* proc) : window((GLFWwindow*)window), procAddress(proc)
	{
		MH_CORE_ASSERT(window, "Window is nullptr!");
	}
	
	void OpenGLContext::Init()
	{
		MH_PROFILE_FUNCTION();

		glfwMakeContextCurrent(window);
		Reload();
	}

	void OpenGLContext::Reload()
	{
		MH_PROFILE_FUNCTION();

		int status = gladLoadGLLoader((GLADloadproc)procAddress);
		MH_CORE_ASSERT(status, "Could not initialize glad!");

		MH_CORE_ASSERT(glGetError, "Could not initialize OpenGL functions!");
	}
	
	void OpenGLContext::SwapBuffers()
	{
		MH_PROFILE_FUNCTION();

		glfwSwapBuffers(window);
	}
}