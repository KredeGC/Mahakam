#pragma once

#include "Mahakam/Renderer/RenderingContext.h"

struct GLFWwindow;

namespace Mahakam
{
	class OpenGLContext : public RenderingContext
	{
	private:
		GLFWwindow* window;
		void* procAddress;

	public:
		OpenGLContext(void* window, void* proc);

		virtual void Init() override;
		virtual void Reload() override;
		virtual void SwapBuffers() override;
	};
}