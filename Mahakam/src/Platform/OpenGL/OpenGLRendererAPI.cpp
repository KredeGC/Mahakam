#include "mhpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Mahakam
{
	void OpenGLRendererAPI::init()
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	const char* OpenGLRendererAPI::getGraphicsVendor()
	{
		return (const char*)glGetString(GL_RENDERER);
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
	{
		glViewport(x, y, w, h);
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

	void OpenGLRendererAPI::enableCulling(bool enable, bool cullFront)
	{
		if (enable)
		{
			glEnable(GL_CULL_FACE);

			if (cullFront)
				glCullFace(GL_FRONT);
			else
				glCullFace(GL_BACK);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

	void OpenGLRendererAPI::setFillMode(bool fill)
	{
		if (fill)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void OpenGLRendererAPI::setBlendMode(bool enable)
	{
		if (enable)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
		}
	}

	void OpenGLRendererAPI::drawIndexed(uint32_t count)
	{
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::drawInstanced(uint32_t indexCount, uint32_t count)
	{
		glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, count);
	}
}