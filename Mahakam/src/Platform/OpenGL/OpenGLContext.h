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

		virtual void Init() override;
		virtual void SwapBuffers() override;
	};
}