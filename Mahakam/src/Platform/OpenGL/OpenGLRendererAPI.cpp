#include "Mahakam/mhpch.h"
#include "OpenGLBase.h"
#include "OpenGLRendererAPI.h"

#include "Mahakam/Core/Profiler.h"

#include <glad/gl.h>

namespace Mahakam
{
	static GLenum BlendModeToOpenGLBlendMode(RendererAPI::BlendMode blendMode)
	{
		switch (blendMode)
		{
		case RendererAPI::BlendMode::Zero:
			return GL_ZERO;
		case RendererAPI::BlendMode::One:
			return GL_ONE;
		case RendererAPI::BlendMode::SrcColor:
			return GL_SRC_COLOR;
		case RendererAPI::BlendMode::SrcAlpha:
			return GL_SRC_ALPHA;
		case RendererAPI::BlendMode::OneMinusSrcColor:
			return GL_ONE_MINUS_SRC_COLOR;
		case RendererAPI::BlendMode::OneMinusSrcAlpha:
			return GL_ONE_MINUS_SRC_ALPHA;
		case RendererAPI::BlendMode::DstColor:
			return GL_DST_COLOR;
		case RendererAPI::BlendMode::DstAlpha:
			return GL_DST_ALPHA;
		}

		MH_BREAK("BlendMode not supported!");

		return 0;
	}

	static GLenum DepthModeToOpenGLDepthMode(RendererAPI::DepthMode depthMode)
	{
		switch (depthMode)
		{
		case RendererAPI::DepthMode::Never:
			return GL_NEVER;
		case RendererAPI::DepthMode::Less:
			return GL_LESS;
		case RendererAPI::DepthMode::LEqual:
			return GL_LEQUAL;
		case RendererAPI::DepthMode::Equal:
			return GL_EQUAL;
		case RendererAPI::DepthMode::NotEqual:
			return GL_NOTEQUAL;
		case RendererAPI::DepthMode::GEqual:
			return GL_GEQUAL;
		case RendererAPI::DepthMode::Greater:
			return GL_GREATER;
		case RendererAPI::DepthMode::Always:
			return GL_ALWAYS;
		}

		MH_BREAK("DepthMode not supported!");

		return 0;
	}

#ifdef MH_ENABLE_GL_ERRORS
	static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		if (type == GL_DEBUG_TYPE_PERFORMANCE)
			MH_WARN("[OpenGL Performance] {0}", message);
		else if (type != GL_DEBUG_TYPE_OTHER)
			MH_ERROR("[OpenGL Error] {0}", message);
	}
#endif

	void OpenGLRendererAPI::Init()
	{
		MH_PROFILE_FUNCTION();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

#ifdef MH_ENABLE_GL_ERRORS
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif
	}

	const char* OpenGLRendererAPI::GetGraphicsVendor()
	{
		return (const char*)glGetString(GL_RENDERER);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool scissor)
	{
		MH_GL_CALL(glViewport(x, y, w, h));

		if (scissor != m_ScissorEnabled)
		{
			m_ScissorEnabled = scissor;
			if (scissor)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
		}

		if (scissor)
			glScissor(x, y, w, h);
	}

	void OpenGLRendererAPI::FinishRendering()
	{
		MH_GL_CALL(glFinish());
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear(bool color, bool depth)
	{
		glClear(
			(color ? GL_COLOR_BUFFER_BIT : 0x00) |
			(depth ? GL_DEPTH_BUFFER_BIT : 0x00));
	}

	void OpenGLRendererAPI::EnableCulling(bool enable, bool cullFront)
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

	void OpenGLRendererAPI::EnableZWriting(bool enable)
	{
		glDepthMask(enable ? GL_TRUE : GL_FALSE);
	}

	void OpenGLRendererAPI::SetZTesting(DepthMode mode)
	{
		// https://www.khronos.org/opengl/wiki/Common_Mistakes#Disable_depth_test_and_allow_depth_writes
		/*if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);*/

		glDepthFunc(DepthModeToOpenGLDepthMode(mode));
	}

	void OpenGLRendererAPI::SetFillMode(bool fill)
	{
		if (fill)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	void OpenGLRendererAPI::SetBlendMode(BlendMode src, BlendMode dst, bool enable)
	{
		GLenum srcBlend = BlendModeToOpenGLBlendMode(src);
		GLenum dstBlend = BlendModeToOpenGLBlendMode(dst);

		if (enable)
		{
			glEnable(GL_BLEND);
			glBlendFunc(srcBlend, dstBlend);
		}
		else
		{
			glDisable(GL_BLEND);
			glBlendFunc(srcBlend, dstBlend);
		}
	}

	void OpenGLRendererAPI::DrawIndexed(uint32_t count)
	{
		MH_GL_CALL(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
	}

	void OpenGLRendererAPI::DrawInstanced(uint32_t indexCount, uint32_t count)
	{
		MH_GL_CALL(glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, count));
	}
}