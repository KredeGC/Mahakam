#include "mhpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Mahakam
{
	void OpenGLRendererAPI::init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		glViewport(x, y, w, h);
	}

	const char* OpenGLRendererAPI::getGraphicsVendor()
	{
		return (const char*)glGetString(GL_RENDERER);
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4 color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::clear(bool color, bool depth)
	{
		glClear(
			(color ? GL_COLOR_BUFFER_BIT : 0x00) |
			(depth ? GL_DEPTH_BUFFER_BIT : 0x00));
	}

	void OpenGLRendererAPI::drawIndexed(uint32_t count)
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
}