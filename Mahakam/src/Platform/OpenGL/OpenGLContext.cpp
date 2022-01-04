#include "mhpch.h"
#include "OpenGLContext.h"
#include <GLFW/glfw3.h>

#include <glad/glad.h>

namespace Mahakam
{
	OpenGLContext::OpenGLContext(GLFWwindow* window) : window(window)
	{
		MH_CORE_ASSERT(window, "Window is nullptr!");
	}
	
	void OpenGLContext::init()
	{
		MH_PROFILE_FUNCTION();

		glfwMakeContextCurrent(window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		MH_ASSERT(status, "Could not initialize glad!");
	}
	
	void OpenGLContext::swapBuffers()
	{
		MH_PROFILE_FUNCTION();

		glfwSwapBuffers(window);
	}
}