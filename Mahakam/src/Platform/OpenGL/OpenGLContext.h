#pragma once

#include "Mahakam/Renderer/RenderingContext.h"

struct GLFWwindow;

namespace Mahakam
{
	class OpenGLContext : public RenderingContext
	{
	private:
		GLFWwindow* window;

	public:
		OpenGLContext(GLFWwindow* window);

		virtual void init() override;
		virtual void swapBuffers() override;
	};
}